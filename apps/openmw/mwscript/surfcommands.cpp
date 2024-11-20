#include "surfcommands.hpp"

#include <components/compiler/opcodes.hpp>
#include "../mwbase/environment.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwphysics/surfmovements.hpp"

#include <components/interpreter/interpreter.hpp>
#include <components/interpreter/runtime.hpp>
#include <components/interpreter/opcodes.hpp>


namespace MWScript
{
    namespace Surf
    {
        template<void (*Setter)(float), float (*Getter)()>
        class OpGetSet : public Interpreter::Opcode0
        {
            public:
                void execute(Interpreter::Runtime& runtime) override
                {
                    // Check if there are parameters by ensuring the program counter is valid
                    bool hasParam = runtime.getPC() >= 0; // Assumes that PC indicates if there are parameters
                    
                    if (hasParam)
                    {
                        // Access the top element directly using operator[]
                        float value = runtime[runtime.getPC()].mFloat; // Accessing the top value
                        runtime.pop(); // Remove the top parameter from the stack
                        Setter(value); // Call Setter with the retrieved value
                        MWBase::Environment::get().getWindowManager()->messageBox("Value set to: " + std::to_string(value));
                    }
                    else
                    {
                        // No parameters, retrieve the current value instead
                        float currentValue = Getter(); 
                        MWBase::Environment::get().getWindowManager()->messageBox("Current value: " + std::to_string(currentValue));
                    }

                }
        };

        void installOpcodes(Interpreter::Interpreter& interpreter)
        {
            //Interpreter::Interpreter& interpreter = MWBase::Environment::get().getScriptManager()->getInterpreter();

            interpreter.installSegment5(Compiler::Misc::opcodeSurfAngleMin,
                new OpGetSet<&MWPhysics::SurfMovements::setSurfAngleMin, &MWPhysics::SurfMovements::getSurfAngleMin>);
            interpreter.installSegment5(Compiler::Misc::opcodeSurfAngleMax,
                new OpGetSet<&MWPhysics::SurfMovements::setSurfAngleMax, &MWPhysics::SurfMovements::getSurfAngleMax>);
            interpreter.installSegment5(Compiler::Misc::opcodeSurfFriction,
                new OpGetSet<&MWPhysics::SurfMovements::setSurfFriction, &MWPhysics::SurfMovements::getSurfFriction>);
            interpreter.installSegment5(Compiler::Misc::opcodeSurfGravity,
                new OpGetSet<&MWPhysics::SurfMovements::setSurfGravity, &MWPhysics::SurfMovements::getSurfGravity>);
            interpreter.installSegment5(Compiler::Misc::opcodeAirAccelerate,
                new OpGetSet<&MWPhysics::SurfMovements::setAirAccelerate, &MWPhysics::SurfMovements::getAirAccelerate>);
            interpreter.installSegment5(Compiler::Misc::opcodeMaxAirSpeed,
                new OpGetSet<&MWPhysics::SurfMovements::setMaxAirSpeed, &MWPhysics::SurfMovements::getMaxAirSpeed>);
            interpreter.installSegment5(Compiler::Misc::opcodeSurfStickForce,
                new OpGetSet<&MWPhysics::SurfMovements::setSurfStickForce, &MWPhysics::SurfMovements::getSurfStickForce>);
        }
    }
}
