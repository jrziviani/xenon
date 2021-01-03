#ifndef ARCH_FACTORY_H
#define ARCH_FACTORY_H

#include "arch_interface.h"

#include <klib/new.h>
#include <klib/logger.h>
#include <klib/singleton.h>
#include <arch/amd64/amd64_interface.h>

/*
 * arch_factory
 *
 * This class is a singleton and will be used everywhere specific platform
 * dependent code is needed.
 *                                                 [Implementations]
 *    +--------------------+                            +-------+
 *    |     singleton      |       [Interface]       +--| amd64 |
 *    |  +--------------+  |    +----------------+   |  +-------+
 *    |  | arch_factory |  |    | arch_interface |   |  +-------+
 *    |  | +call() -----+--+--->| +init_pci()    |<==+--| ppc64 |
 *    |  |              |  |    | +init_paging() |   |  +-------+
 *    |  +--------------+  |    | +...           |   |  +-------+
 *    +--------------------+    +----------------+   +--|  arm  |
 *                                                      +-------+
 * - Picking the right platform is a task make by the compiler
 *   At the end of this source code there's a small piece of code,
 *   guarded by #ifdefs, that will create an alias to the singleton
 *   arch_factory considering the actual platform.
 *
 * - Calling a platform dependent code is simple:
 *   arch_factory::instance().call()->init_pci();
 *
 *   or, using an alias variable:
 *   auto arch = arch_factory::instance().call();
 *   arch->init_pci();
 *   arch->...();
 *
 * - This approach won't polute the code with lots of C macros, they
 *   will be small and only to solve a particular problem.
 */
enum class ARCHITECTURES
{
    AMD64,
    AARCH64,
    PPC64LE,
};

template <ARCHITECTURES Iface>
class arch_factory__
{
    friend class klib::singleton<arch_factory__>;

    arch_interface *arch_ = nullptr;

private:
    arch_factory__()
    {
        if constexpr (Iface == ARCHITECTURES::AMD64) {
            klib::logger::instance().log("Booting amd64");
            arch_ = new amd64_interface();
        }
        else if constexpr (Iface == ARCHITECTURES::AARCH64) {
            klib::logger::instance().log("Booting Aarch64");
        }
        else if constexpr (Iface == ARCHITECTURES::PPC64LE) {
            klib::logger::instance().log("Booting PowerPC64");
        }
    }

public:
    arch_interface *operator->()
    {
        return arch_;
    }

public:
    ~arch_factory__() = default;
    arch_factory__(const arch_factory__&) = delete;
    arch_factory__(arch_factory__&&) = delete;
    arch_factory__ &operator=(const arch_factory__&) = delete;
    arch_factory__ &&operator=(arch_factory__&&) = delete;
};

#ifdef __x86_64__
using arch_factory = klib::singleton<arch_factory__<ARCHITECTURES::AMD64>>;
#elif __aarch64__
using arch_factory = klib::singleton<arch_factory__<ARCHITECTURES::AARCH64>>;
#elif __ppc64__
using arch_factory = klib::singleton<arch_factory__<ARCHITECTURES::PPC64LE>>;
#endif

#endif // ARCH_FACTORY_H
