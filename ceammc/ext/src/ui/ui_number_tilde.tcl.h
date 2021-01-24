// this is autogenerated file: do not edit
// clang-format off
#ifndef ui_number_tilde_tcl_h_
#define ui_number_tilde_tcl_h_
const char* ui_number_tilde_tcl = 
"namespace eval ui {\n"
"proc number_tag {id} { return \"${id}_#all\" }\n"
"proc number_font { zoom } { return \"Helvetica [expr $zoom * 11] normal roman\" }\n"
"proc number_draw_box {c tag w h zoom bdcolor acolor tcolor value} {\n"
"    set x [expr $h * 0.7]\n"
"    set ft [number_font $zoom]\n"
"    $c create line $x 0 $x $h -width 1 -fill $bdcolor -tags $tag\n"
"    set tx [expr $x * 0.5]\n"
"    set ty [expr $h * 0.5]\n"
"    $c create text $tx $ty -text \"~\" -anchor center -justify center \\\n"
"        -font $ft -fill $acolor -width 0 -tags $tag\n"
"    set tx [expr $h * 0.9]\n"
"    set ty [expr $h * 0.55]\n"
"    $c create text $tx $ty -text $value -anchor w -justify left \\\n"
"        -font $ft -fill $tcolor -width 0 -tags $tag\n"
"}\n"
"proc number_update {cnv id w h zoom bdcolor acolor tcolor value} {\n"
"    set c [::ceammc::ui::widget_canvas $cnv $id]\n"
"    set tag [number_tag $id]\n"
"    $c delete $tag\n"
"    number_draw_box $c $tag $w $h $zoom $bdcolor $acolor $tcolor $value\n"
"}\n"
"}\n"
;
#endif
// clang-format on