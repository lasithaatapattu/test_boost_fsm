#include <iostream>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>
namespace msm = boost::msm;
namespace msmf = boost::msm::front;
namespace mpl = boost::mpl; // Boost.MPL is used extensively by Boost.MSM

// Events
struct TimerExpires {};
struct CarDetected {};

// States
struct Red : public msmf::state<> {
    template <class Event, class FSM>
    void on_entry(Event const&, FSM&) const {
        std::cout << "Entering Red state." << std::endl;
    }
    template <class Event, class FSM>
    void on_exit(Event const&, FSM&) const {
        std::cout << "Leaving Red state." << std::endl;
    }
};

struct Yellow : public msmf::state<> {
    template <class Event, class FSM>
    void on_entry(Event const&, FSM&) const {
        std::cout << "Entering Yellow state." << std::endl;
    }
    template <class Event, class FSM>
    void on_exit(Event const&, FSM&) const {
        std::cout << "Leaving Yellow state." << std::endl;
    }
};

struct Green : public msmf::state<> {
    template <class Event, class FSM>
    void on_entry(Event const&, FSM&) const {
        std::cout << "Entering Green state." << std::endl;
    }
    template <class Event, class FSM>
    void on_exit(Event const&, FSM&) const {
        std::cout << "Leaving Green state." << std::endl;
    }
};

// Define the State Machine
struct TrafficLight_ : public msmf::state_machine_def<TrafficLight_> {
    typedef Red initial_state;

    // Actions (optional): Functions or function objects executed during a transition
    struct TurnOnGreenLight {
        template <class Event, class FSM, class SourceState, class TargetState>
        void operator()(Event const&, FSM&, SourceState&, TargetState&) const {
            std::cout << "Action: Turning on Green Light." << std::endl;
        }
    };

    struct TurnOnYellowLight {
        template <class Event, class FSM, class SourceState, class TargetState>
        void operator()(Event const&, FSM&, SourceState&, TargetState&) const {
            std::cout << "Action: Turning on Yellow Light." << std::endl;
        }
    };

    struct TurnOnRedLight {
        template <class Event, class FSM, class SourceState, class TargetState>
        void operator()(Event const&, FSM&, SourceState&, TargetState&) const {
            std::cout << "Action: Turning on Red Light." << std::endl;
        }
    };

    // Guards (optional): Predicates that must be true for a transition to occur
    struct IsCarWaiting {
        template <class Event, class FSM, class SourceState, class TargetState>
        bool operator()(Event const&, FSM&, SourceState&, TargetState&) const {
            // For simplicity, always return true. In a real system, you'd check a sensor.
            return true; 
        }
    };

    // Transition table: Defines the state transitions
    // Row<SourceState, Event, TargetState, Action, Guard>
    struct transition_table : mpl::vector<
        //  Source       Event           Target      Action              Guard
        msmf::Row< Red,         TimerExpires,   Green,      TurnOnGreenLight,   msmf::none >,
        msmf::Row< Green,       TimerExpires,   Yellow,     TurnOnYellowLight,  msmf::none >,
        msmf::Row< Green,       CarDetected,    Red,        TurnOnRedLight,     IsCarWaiting >, // If car detected, go to Red immediately
        msmf::Row< Yellow,      TimerExpires,   Red,        TurnOnRedLight,     msmf::none   >
    > {};

    // Optional: Define a no_transition handler for unhandled events
    template <class Event, class Fsm>
    void no_transition(Event const& e, Fsm&, int state) {
        std::cout << "No transition from state " << state << " on event " 
                  << typeid(e).name() << std::endl;
    }
};

// Pick a back-end for the state machine
typedef msm::back::state_machine<TrafficLight_> TrafficLight;

// Helper to print current state (for debugging)
static char const* const state_names[] = { "Red", "Yellow", "Green" };
void print_current_state(TrafficLight const& tl) {
    std::cout << "Current State: " << state_names[tl.current_state()[0]] << std::endl;
}

int main() {
    TrafficLight traffic_light;

    // Start the state machine
    traffic_light.start();
    print_current_state(traffic_light);

    std::cout << "\nProcessing TimerExpires event (Red -> Green)..." << std::endl;
    traffic_light.process_event(TimerExpires());
    print_current_state(traffic_light);

    std::cout << "\nProcessing TimerExpires event (Green -> Yellow)..." << std::endl;
    traffic_light.process_event(TimerExpires());
    print_current_state(traffic_light);

    std::cout << "\nProcessing TimerExpires event (Yellow -> Red)..." << std::endl;
    traffic_light.process_event(TimerExpires());
    print_current_state(traffic_light);

    std::cout << "\nProcessing CarDetected event (Red -> No transition, will print error)..." << std::endl;
    traffic_light.process_event(CarDetected()); // No transition from Red on CarDetected
    print_current_state(traffic_light);

    std::cout << "\nResetting to Green to demonstrate CarDetected transition..." << std::endl;

    traffic_light.process_event(TimerExpires()); // Red -> Green
    traffic_light.process_event(TimerExpires()); // Green -> Yellow
    traffic_light.process_event(TimerExpires()); // Yellow -> Red
    traffic_light.process_event(TimerExpires()); // Red -> Green (back to Green)
    print_current_state(traffic_light);

    std::cout << "\nProcessing CarDetected event (Green -> Red)..." << std::endl;
    traffic_light.process_event(CarDetected());
    print_current_state(traffic_light);
    
    // Stop the state machine
    traffic_light.stop();

    return 0;
}