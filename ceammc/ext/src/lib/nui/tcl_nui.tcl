namespace eval ::nui {

proc pd_canvas { cnv } { return ".x${cnv}.c" }

namespace eval utils {
    proc fix_mac_state {n} { if { $n > 256 } { return [expr $n & 0xFF ] } { return $n } }

    proc fix_win32_alt {n} {
        if { $n & 131072 } {
            incr n -131072
            set n [expr $n | 16]
        }
        return $n
    }

    proc fix_win32_state {n} {
        if { $n & 32 } { incr n -32 }
        return [expr 0xFF & [fix_win32_alt $n]]
    }

    proc send_mouse_move {obj x y mod} {
        switch -- $::windowingsystem {
            "aqua" {
                # disable mouse dragging for buttons other then first
                if { $mod < 512 } { pdsend "$obj mousemove $x $y [fix_mac_state $mod]" }
            } "win32" {
                set mod [fix_win32_alt $mod]
                if { $mod < 512 } { pdsend "$obj mousemove $x $y [fix_win32_state $mod]" }
            } "default" {
                pdsend "$obj mousemove $x $y $mod"
            }
        }
    }
}

proc widget_canvas { cnv id } { return ".x${cnv}.c.cnv${id}" }
proc widget_window { cnv id } { return ".x${cnv}.c.win${id}" }
proc widget_w   { w zoom } { expr $w * $zoom + 1 }
proc widget_h   { h zoom } { expr $h * $zoom + 1 }
proc widget_tag { id } { return "#${id}" }

proc canvas_motion {cnv id val} {
    set c  [pd_canvas $cnv]
    set rx [winfo rootx $c]
    set ry [winfo rooty $c]
    set x  [winfo pointerx .]
    set y  [winfo pointery .]
    pdtk_canvas_motion $c [expr $x - $rx] [expr $y - $ry] $val
}

proc canvas_down {cnv id val} {
    set c  [pd_canvas $cnv]
    set rx [winfo rootx $c]
    set ry [winfo rooty $c]
    set x  [winfo pointerx .]
    set y  [winfo pointery .]
    pdtk_canvas_mouse $c [expr $x - $rx] [expr $y - $ry] 0 $val
}

proc canvas_up {cnv id} {
    set c  [pd_canvas $cnv]
    set rx [winfo rootx $c]
    set ry [winfo rooty $c]
    set x  [winfo pointerx .]
    set y  [winfo pointery .]
    pdtk_canvas_mouseup $c [expr $x - $rx] [expr $y - $ry] 0
}

proc canvas_right {cnv id} {
    set c  [pd_canvas $cnv]
    set rx [winfo rootx $c]
    set ry [winfo rooty $c]
    set x  [winfo pointerx .]
    set y  [winfo pointery .]
    pdtk_canvas_rightclick $c [expr $x - $rx] [expr $y - $ry] 0
}

proc widget_create { cnv id x y w h zoom } {
    set win [widget_window $cnv $id]
    set c [widget_canvas $cnv $id]
    set w [widget_w $w $zoom]
    set h [widget_h $h $zoom]

    namespace eval "nui$id" {}
    destroy $c
    canvas $c -width $w -height $h -bd 0 \
        -highlightthickness 0 -insertborderwidth 0 \
        -state normal -takefocus 1 -insertwidth 0 -confine 0

    $c configure -bg #AAAAAA

    [pd_canvas $cnv] create window $x $y -anchor nw -window $c \
        -tags $win -width $w -height $h
}

proc widget_bg { cnv id color } {
    set c [widget_canvas $cnv $id]
    $c configure -bg $color
}

proc widget_erase { cnv id } {
    set win [widget_window $cnv $id]
    set c [widget_canvas $cnv $id]

    [pd_canvas $cnv] delete $win
    destroy $c
}

proc widget_resize { cnv id w h zoom } {
    set win [widget_window $cnv $id]
    set c [widget_canvas $cnv $id]

    set w [widget_w $w $zoom]
    set h [widget_h $h $zoom]

    $c configure -width $w -height $h
    [pd_canvas $cnv] itemconfigure $win -width $w -height $h
}

proc widget_move { cnv id x y } {
    set c [pd_canvas $cnv]
    set win [widget_window $cnv $id]
    $c coords $win $x $y
}

proc widget_focus { cnv id } {
    focus [widget_canvas $cnv $id]
}

proc widget_mouse_bind { cnv id target args }  {
    set c [widget_canvas $cnv $id]
    foreach name $args {
        set ev "widget_mouse_${name}_bind"
        $ev $c "#$target"
    }
}

proc widget_cursor { cnv id name }  {
    set c [widget_canvas $cnv $id]
    $c configure -cursor $name
}

proc widget_mouse_move_bind  {id obj} { bind $id <Motion> [subst {+::nui::utils::send_mouse_move $obj %x %y %s}] }
proc widget_mouse_enter_bind {id obj} { bind $id <Enter>  [subst {+pdsend "$obj mouseenter"}] }
proc widget_mouse_leave_bind {id obj} { bind $id <Leave>  [subst {+pdsend "$obj mouseleave"}] }

proc widget_mouse_down_bind {id obj} {
    switch -- $::windowingsystem {
        "win32" {
            bind $id <ButtonPress-1> [subst -nocommands {+pdsend "$obj mousedown %x %y %X %Y [ceammc_fix_win32_state %s]"}]
        } "default" {
            bind $id <ButtonPress-1> [subst {+pdsend "$obj mousedown %x %y %X %Y %s"}]
        }
    }
}

# sends #obj mouseup X Y MOD
proc widget_mouse_up_bind {id obj} {
    switch -- $::windowingsystem {
        "aqua" {
            bind $id <ButtonRelease-1> [subst -nocommands {+pdsend "$obj mouseup %x %y [ceammc_fix_macos_state %s]"}]
        } "win32" {
            bind $id <ButtonRelease-1> [subst -nocommands {+pdsend "$obj mouseup %x %y [ceammc_fix_win32_state %s]"}]
        } "default" {
            bind $id <ButtonRelease-1> [subst {+pdsend "$obj mouseup %x %y %s"}]
        }
    }
}

# sends #obj rightclick X Y ABSX ABSY MOD
proc widget_mouse_right_bind {id obj} {
    switch -- $::windowingsystem {
        "aqua" {
            bind $id <ButtonPress-2> [subst {+pdsend "$obj rightclick %x %y %X %Y %s"}]
            bind $id <Control-ButtonPress-1> [subst {+pdsend "$obj rightclick %x %y %X %Y %s"}]
        } "win32" {
            bind $id <ButtonPress-3> [subst -nocommands {+pdsend "$obj rightclick %x %y %X %Y [ceammc_fix_win32_state %s]"}]
        } "default" {
            bind $id <ButtonPress-3> [subst {+pdsend "$obj rightclick %x %y %X %Y %s"}]
        }
    }
}

if { [catch {package require tooltip} ] } {
    proc widget_tooltip { cnv model id msg } {}
} {
    proc widget_tooltip { cnv model id msg } {
        set c [widget_canvas $cnv $model]
        tooltip::tooltip $c -item $id $msg
    }
}

proc item_erase { cnv model tags } {
    set c [widget_canvas $cnv $model]
    $c delete $tags
}

proc item_move { cnv model tag x y } {
    set c [widget_canvas $cnv $model]

    lassign [$c coords $tag] x0 y0 x1 y1
    set w [expr $x1-$x0]
    set h [expr $y1-$y0]
    $c coords $tag $x $y [expr $x+$w] [expr $y+$h]
}

namespace eval frame {
    proc tag { id } { return "#f${id}" }

    proc create { cnv model id x y w h border_color fill_color width } {
        set c [::nui::widget_canvas $cnv $model]
        set t [tag $id]
        $c create rectangle $x $y [expr $x+$w] [expr $y+$h] \
            -fill $fill_color -outline $border_color -width $width -tags $t
    }

    proc update { cnv model id w h border_color fill_color } {
        set c [::nui::widget_canvas $cnv $model]
        set t [tag $id]
        $c itemconfigure $t -outline $border_color -fill $fill_color
        lassign [$c coords $t] x0 y0 x1 y1
        $c coords $t $x0 $y0 [expr $x0+$w] [expr $y0+$h]
    }

    proc erase { cnv model id } {
        ::nui::item_erase $cnv $model [tag $id]
    }

    proc move  { cnv model id x y } {
        ::nui::item_move $cnv $model [tag $id] $x $y
    }
}

namespace eval label {
    proc tag { id } { return "#l${id}" }

    proc create { cnv model id x y width anchor font color txt } {
        set c [::nui::widget_canvas $cnv $model]
        set t [tag $id]

        $c create text $x $y -fill $color -text $txt -font $font -anchor $anchor -width $width -tags $t
    }

    proc erase { cnv model id } {
        ::nui::item_erase $cnv $model [tag $id]
    }

    proc move  { cnv model id x y } {
        ::nui::item_move $cnv $model [tag $id] $x $y
    }
}

namespace eval slider {
    proc tag_all  { id } { return "#sl${id}" }
    proc tag_box  { id } { return "#sl${id}_box" }
    proc tag_knob { id } { return "#sl${id}_kn" }

    proc knob_xpos { x w pos } { return [expr ($w*$pos)+$x] }

    proc hcreate { cnv model id x y w h zoom pos value out_color fill_color knob_color } {
        set c [::nui::widget_canvas $cnv $model]
        set ta [tag_all $id]
        set tb [tag_box $id]
        set tkn [tag_knob $id]

        # draw box
        $c create rectangle $x $y [expr $x+$w] [expr $y+$h] \
            -fill $fill_color -outline $out_color -width $zoom -tags [list $ta $tb]

        # draw knob
        set kx [knob_xpos $x $w $pos]
        set ky0 [expr $y+1]
        set ky1 [expr $y+$h-1]
        $c create line $kx $ky0 $kx $ky1 \
            -fill $knob_color -width [expr 2*$zoom] -tags [list $ta $tkn]

        ::nui::widget_tooltip $cnv $model $tkn "value: $value"
    }

    proc hupdate { cnv model id pos value out_color fill_color knob_color } {
        set c [::nui::widget_canvas $cnv $model]
        set tb [tag_box $id]
        set tkn [tag_knob $id]

        # update box
        $c itemconfigure $tb -fill $fill_color -outline $out_color

        lassign [$c coords $tb] x0 y0 x1 y1
        set w [expr $x1-$x0]
        set x [knob_xpos $x0 $w $pos]

        # update knob
        lassign [$c coords $tkn] x0 y0 x1 y1
        $c coords $tkn $x $y0 $x $y1
        $c itemconfigure $tkn -fill $knob_color
    }

    proc erase { cnv model id } {
        set ta [tag_all $id]
        ::nui::item_erase $cnv $model $ta
    }

    proc move  { cnv model id x y } {
        set c [::nui::widget_canvas $cnv $model]
        set tb [tag_box $id]
        set tkn [tag_knob $id]


        lassign [$c coords $tb] x0 y0 x1 y1
        set w [expr $x1-$x0]
        set h [expr $y1-$y0]

        $c coords $tb $x $y [expr $x+$w] [expr $y+$h]

        set dx [expr $x0-$x]
        set dy [expr $y0-$y]
        lassign [$c coords $tkn] x0 y0 x1 y1
        $c coords $tkn [expr $x+$dx] [expr $y+$dy]
    }
}

}