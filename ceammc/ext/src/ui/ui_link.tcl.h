// this is autogenerated file: do not edit
// clang-format off
#ifndef ui_link_tcl_h_
#define ui_link_tcl_h_
const char* ui_link_tcl = 
"namespace eval ::ui {\n"
"namespace eval link {\n"
"    variable font_size 11\n"
"    variable font_family \"Helvetica\"\n"
"}\n"
"switch -- $::windowingsystem {\n"
"    \"aqua\" {\n"
"        set link::font_family \"Menlo\"\n"
"    }\n"
"    \"x11\"  {\n"
"        set link::font_family \"Helvetica\"\n"
"    }\n"
"    \"win32\" {\n"
"        set link::font_family \"Dejavu Sans Mono\"\n"
"        set link::font_size 7\n"
"    }\n"
"}\n"
"proc link_type_font { zoom } { return \"{$link::font_family} [expr $zoom * $link::font_size] normal roman\" }\n"
"proc link_open {filename dir} {\n"
"    if {[string first \"://\" $filename] > -1} {\n"
"        menu_openfile $filename\n"
"    } elseif {[file pathtype $filename] eq \"absolute\"} {\n"
"        if {[file extension $filename] eq \".pd\"} {\n"
"            set dir [file dirname $filename]\n"
"            set name [file tail $filename]\n"
"            menu_doc_open $dir $name\n"
"        } else {\n"
"            menu_openfile $filename\n"
"        }\n"
"    } elseif {[file exists [file join $dir $filename]]} {\n"
"        set fullpath [file normalize [file join $dir $filename]]\n"
"        set dir [file dirname $fullpath]\n"
"        set filename [file tail $fullpath]\n"
"        menu_doc_open $dir $filename\n"
"   } elseif {[file exists [file join $::sys_libdir doc 5.reference $filename]]} {\n"
"        # search in core doc dir\n"
"        set fullpath [file normalize [file join $::sys_libdir doc 5.reference $filename]]\n"
"        set dir [file dirname $fullpath]\n"
"        set filename [file tail $fullpath]\n"
"        menu_doc_open $dir $filename\n"
"    } elseif {[file exists [file join $::sys_libdir extra $filename]]} {\n"
"        # search in external doc dir\n"
"        set fullpath [file normalize [file join $::sys_libdir extra $filename]]\n"
"        set dir [file dirname $fullpath]\n"
"        set filename [file tail $fullpath]\n"
"        menu_doc_open $dir $filename\n"
"    } else {\n"
"        bell ; # beep on error to provide instant feedback\n"
"        pdtk_post \"ERROR: file not found: $filename at $dir\"\n"
"        pdtk_post \"\"\n"
"    }\n"
"}\n"
"proc link_update {cnv id rid w h zoom txt_color hover_color url txt} {\n"
"    set c [::ceammc::ui::widget_canvas $cnv $id]\n"
"    set t [::ceammc::ui::widget_tag $id]\n"
"    $c delete $t\n"
"    set x 0\n"
"    set txpad [expr $zoom*2+1]\n"
"    set typad [expr $zoom*2+1]\n"
"    set tx $txpad\n"
"    set ty $typad\n"
"    set ft [link_type_font $zoom]\n"
"    $c create text $tx $ty -text $txt -anchor nw -justify left \\\n"
"        -font $ft -fill $txt_color -width 0 -tags $t\n"
"    # bind mouse events\n"
"    set onenter \"$c itemconfigure $t -fill $hover_color\"\n"
"    bind $c <Enter> $onenter\n"
"    set onleave \"$c itemconfigure $t -fill $txt_color\"\n"
"    bind $c <Leave> $onleave\n"
"    # tooltip\n"
"    ceammc_tooltip $c $url\n"
"    # calc text bbox\n"
"    lassign [$c bbox $t] tx0 ty0 tx1 ty1\n"
"    set tw [expr $tx1 - $tx0]\n"
"    set th [expr abs($ty1 - $ty0)]\n"
"    # calc new width\n"
"    set nw $w\n"
"    set dispw [expr $tw + (1*$txpad)]\n"
"    if { $dispw != $nw } { set nw $dispw }\n"
"    # calc new height\n"
"    set nh 5\n"
"    set disph [expr $th + $typad]\n"
"    if { $disph > $nh } { set nh $disph }\n"
"    if { $nh != $h || $nw != $w } {\n"
"        pdsend \"$rid .resize $nw $nh\"\n"
"    }\n"
"}\n"
"}\n"
;
#endif
// clang-format on
