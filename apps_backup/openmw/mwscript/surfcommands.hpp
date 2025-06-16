#ifndef GAME_SCRIPT_SURFCOMMANDS_H
#define GAME_SCRIPT_SURFCOMMANDS_H

#include "../mwbase/environment.hpp"
#include "../mwworld/ptr.hpp"
#include <components/compiler/extensions.hpp>
#include <components/compiler/opcodes.hpp>

namespace Compiler
{
    class Extensions;
}

namespace Interpreter
{
    class Interpreter;
}

namespace MWScript
{
    namespace Surf
    {
  /*      void registerExtensions(Compiler::Extensions& extensions);*/
        void installOpcodes (Interpreter::Interpreter& interpreter);
    }
}

#endif
