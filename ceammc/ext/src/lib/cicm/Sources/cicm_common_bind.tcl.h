// this is autogenerated file: do not edit
// clang-format off
#ifndef cicm_common_bind_tcl_h_
#define cicm_common_bind_tcl_h_
constexpr const char* cicm_common_bind_tcl = 
"proc ceammc_fix_macos_state {n} { if { $n > 256 } { return [expr $n & 0xFF ] } { return $n } }\n"
"proc ceammc_fix_win32_alt {n} {\n"
"    if { $n & 131072 } {\n"
"        incr n -131072\n"
"        set n [expr $n | 16]\n"
"    }\n"
"    return $n\n"
"}\n"
"proc ceammc_fix_win32_state {n} {\n"
"    if { $n & 32 } { incr n -32 }\n"
"    return [expr 0xFF & [ceammc_fix_win32_alt $n]]\n"
"}\n"
"proc ceammc_fix_x11_state {n} {}\n"
"proc ceammc_send_motion {obj x y mod} {\n"
"    switch -- $::windowingsystem {\n"
"        \"aqua\" {\n"
"            # disable mouse dragging for buttons other then first\n"
"            if { $mod < 512 } { pdsend \"$obj mousemove $x $y [ceammc_fix_macos_state $mod]\" }\n"
"        } \"win32\" {\n"
"            set mod [ceammc_fix_win32_alt $mod]\n"
"            if { $mod < 512 } { pdsend \"$obj mousemove $x $y [ceammc_fix_win32_state $mod]\" }\n"
"        } \"default\" {\n"
"            pdsend \"$obj mousemove $x $y $mod\"\n"
"        }\n"
"    }\n"
"}\n"
"proc ceammc_bind_mouse_down {id obj} {\n"
"    switch -- $::windowingsystem {\n"
"        \"win32\" {\n"
"            bind $id <ButtonPress-1> [subst -nocommands {+pdsend \"$obj mousedown %x %y %X %Y [ceammc_fix_win32_state %s]\"}]\n"
"        } \"default\" {\n"
"            bind $id <ButtonPress-1> [subst {+pdsend \"$obj mousedown %x %y %X %Y %s\"}]\n"
"        }\n"
"    }\n"
"}\n"
"proc ceammc_bind_mouse_up {id obj} {\n"
"    switch -- $::windowingsystem {\n"
"        \"aqua\" {\n"
"            bind $id <ButtonRelease-1> [subst -nocommands {+pdsend \"$obj mouseup %x %y [ceammc_fix_macos_state %s]\"}]\n"
"        } \"win32\" {\n"
"            bind $id <ButtonRelease-1> [subst -nocommands {+pdsend \"$obj mouseup %x %y [ceammc_fix_win32_state %s]\"}]\n"
"        } \"default\" {\n"
"            bind $id <ButtonRelease-1> [subst {+pdsend \"$obj mouseup %x %y %s\"}]\n"
"        }\n"
"    }\n"
"}\n"
"proc ceammc_bind_mouse_double_click {id obj} {\n"
"    switch -- $::windowingsystem {\n"
"        \"aqua\" {\n"
"            bind $id <Double-Button-1> [subst -nocommands {+pdsend \"$obj dblclick %x %y [ceammc_fix_macos_state %s]\"}]\n"
"        } \"win32\" {\n"
"            bind $id <Double-Button-1> [subst -nocommands {+pdsend \"$obj dblclick %x %y [ceammc_fix_win32_state %s]\"}]\n"
"        } \"default\" {\n"
"            bind $id <Double-Button-1> [subst {+pdsend \"$obj dblclick %x %y %s\"}]\n"
"        }\n"
"    }\n"
"}\n"
"proc ceammc_bind_mouse_right_click {id obj} {\n"
"    switch -- $::windowingsystem {\n"
"        \"aqua\" {\n"
"            bind $id <ButtonPress-2> [subst {+pdsend \"$obj rightclick %x %y %X %Y %s\"}]\n"
"            bind $id <Control-ButtonPress-1> [subst {+pdsend \"$obj rightclick %x %y %X %Y %s\"}]\n"
"        } \"win32\" {\n"
"            bind $id <ButtonPress-3> [subst -nocommands {+pdsend \"$obj rightclick %x %y %X %Y [ceammc_fix_win32_state %s]\"}]\n"
"        } \"default\" {\n"
"            bind $id <ButtonPress-3> [subst {+pdsend \"$obj rightclick %x %y %X %Y %s\"}]\n"
"        }\n"
"    }\n"
"}\n"
"proc ceammc_bind_mouse_wheel {id obj} {\n"
"    switch -- $::windowingsystem {\n"
"        \"win32\" {\n"
"            bind $id <MouseWheel> [subst -nocommands {+pdsend \"$obj mousewheel %x %y [expr %D / 120.0] [ceammc_fix_win32_state %s]\"}]\n"
"        } \"default\" {\n"
"            bind $id <MouseWheel> [subst {+pdsend \"$obj mousewheel %x %y %D %s\"}]\n"
"        }\n"
"    }\n"
"}\n"
"proc ceammc_bind_mouse_move {id obj} { bind $id <Motion> [subst {+ceammc_send_motion $obj %x %y %s}] }\n"
"proc ceammc_bind_mouse_enter {id obj} { bind $id <Enter> [subst {+pdsend \"$obj mouseenter\"}] }\n"
"proc ceammc_bind_mouse_leave {id obj} { bind $id <Leave> [subst {+pdsend \"$obj mouseleave\"}] }\n"
;
#endif
// clang-format on
