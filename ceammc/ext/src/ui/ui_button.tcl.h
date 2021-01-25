// this is autogenerated file: do not edit
// clang-format off
#ifndef ui_button_tcl_h_
#define ui_button_tcl_h_
const char* ui_button_tcl = 
"namespace eval ::ui {\n"
"proc button_tag {id} { return  ${id}_#all }\n"
"proc button_update {cnv id w h zoom c0 c1 state} {\n"
"    set c [::ceammc::ui::widget_canvas $cnv $id]\n"
"    set t [button_tag $id]\n"
"    $c delete $t\n"
"    if { $state == 1 } { set color $c1 } { set color $c0 }\n"
"    set xpad [expr {round($w/10)} + 1 + $zoom]\n"
"    set ypad [expr {round($h/10)} + 1 + $zoom]\n"
"    set x0 [expr $xpad]\n"
"    set x1 [expr $w - $xpad]\n"
"    set y0 [expr $ypad]\n"
"    set y1 [expr $h - $ypad]\n"
"    $c create rectangle $x0 $y0 $x1 $y1 -fill $color -outline $color -width $zoom -tags $t\n"
"}\n"
"}\n"
;
#endif
// clang-format on
