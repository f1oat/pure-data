# PATCHER MOUSE MOTION
proc eobj_canvas_motion {patcher val} {
    set rx [winfo rootx $patcher]
    set ry [winfo rooty $patcher]
    set x  [winfo pointerx .]
    set y  [winfo pointery .]
    pdtk_canvas_motion $patcher [expr $x - $rx] [expr $y - $ry] $val
}

# PATCHER MOUSE DOWN
proc eobj_canvas_down {patcher val} {
    set rx [winfo rootx $patcher]
    set ry [winfo rooty $patcher]
    set x  [winfo pointerx .]
    set y  [winfo pointery .]
    pdtk_canvas_mouse $patcher [expr $x - $rx] [expr $y - $ry] 0 $val
}

# PATCHER MOUSE UP
proc eobj_canvas_up {patcher} {
    set rx [winfo rootx $patcher]
    set ry [winfo rooty $patcher]
    set x  [winfo pointerx .]
    set y  [winfo pointery .]
    pdtk_canvas_mouseup $patcher [expr $x - $rx] [expr $y - $ry] 0
}

# PATCHER MOUSE RIGHT
proc eobj_canvas_right {patcher} {
    set rx [winfo rootx $patcher]
    set ry [winfo rooty $patcher]
    set x  [winfo pointerx .]
    set y  [winfo pointery .]
    pdtk_canvas_rightclick $patcher [expr $x - $rx] [expr $y - $ry] 0
}

# OBJECT SAVE FILE
proc eobj_saveas {name initialfile initialdir} {
    if { ! [file isdirectory $initialdir]} {set initialdir $::env(HOME)}
    set filename [tk_getSaveFile -initialfile $initialfile -initialdir $initialdir -filetypes $::filetypes]
    if {$filename eq ""}
        return;

    set extension [file extension $filename]
    set oldfilename $filename

    if {$filename ne $oldfilename && [file exists $filename]} {
        set answer [tk_messageBox -type okcancel -icon question -default cancel-message [_ "$filename" already exists. Do you want to replace it?]]
        if {$answer eq "cancel"}
            return;
    }
    set dirname [file dirname $filename]
    set basename [file tail $filename]
    pdsend "$name eobjwriteto [enquote_path $dirname/$basename]"
    set ::filenewdir $dirname
}

# OBJECT OPEN FILE
proc eobj_openfrom {name} {
    if { ! [file isdirectory $::filenewdir]} {
    set ::filenewdir [file normalize $::env(HOME)]
    }
    set files [tk_getOpenFile -multiple false -initialdir $::fileopendir]
    pdsend "$name eobjreadfrom [enquote_path $files]"
}

# RGBA TO HEX
proc eobj_rgba_to_hex {red green blue alpha} {
    set nR [expr int( $red * 65025 )]
    set nG [expr int( $green * 65025 )]
    set nB [expr int( $blue * 65025 )]
    set col [format {%4.4x} $nR]
    append col [format {%4.4x} $nG]
    append col [format {%4.4x} $nB]
    return #$col
}

# RGB int list to RGB float list
proc eobj_rgb_int_to_float {red green blue} {
    set nR [expr ( $red / 65025. )]
    set nG [expr ( $green / 65025. )]
    set nB [expr ( $blue / 65025. )]
    if {$nR > 1.} {set nR 1.}
    if {$nG > 1.} {set nG 1.}
    if {$nB > 1.} {set nB 1.}
    return [concat $nR $nG $nB]
}

proc font_create_label_bold {font_name} {
    set font [ttk::style lookup TLabel -font]
    set family [font configure $font -family]
    set size [font configure $font -size]
    set slant [font configure $font -slant]
    set underline [font configure TkDefaultFont -underline]
    return [font create $font_name -family $family -size $size -weight bold -slant $slant -underline $underline]
}
