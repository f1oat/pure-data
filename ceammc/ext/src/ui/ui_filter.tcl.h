// this is autogenerated file: do not edit
// clang-format off
#ifndef ui_filter_tcl_h_
#define ui_filter_tcl_h_
const char* ui_filter_tcl = 
"namespace eval ui {\n"
"namespace eval filter {\n"
"    variable db_min -24.0\n"
"    variable db_max 24.0\n"
"    variable db_range [expr $db_max - $db_min]\n"
"    variable db_labels_short  { -12 0 12 }\n"
"    variable db_labels_long   { -18 -12 -6 0 6 12 18 }\n"
"    variable freq_min 10.0\n"
"    variable freq_max 20000.0\n"
"    variable freq_nyq 22050.0\n"
"    variable freq_steps { 500 1000 2000 2500 5000 10000 }\n"
"    variable fmin_log10 [expr log10($freq_min)]\n"
"    variable fmax_log10 [expr log10($freq_max)]\n"
"    proc clip { v min max } {\n"
"        if {$v < $min} { return $min }\n"
"        if {$v > $max} { return $max }\n"
"        return $v\n"
"    }\n"
"    proc finite {x} {\n"
"        expr {[string is double -strict $x] && $x == $x && $x + 1 != $x}\n"
"    }\n"
"}\n"
"proc filter_font { zoom } { return \"Helvetica [expr $zoom * 7] normal roman\" }\n"
"proc filter_draw_hdb { c t w h zoom labels color txtcolor } {\n"
"    set ft [filter_font $zoom]\n"
"    set lx [expr $w - (2*$zoom)]\n"
"    set t0 [lindex $labels 0]\n"
"    foreach l $labels {\n"
"        set y [expr ($h/$::ui::filter::db_range)*$l + $h*0.5]\n"
"        # accent zero level line\n"
"        if { $l == 0 } { set lc [::tk::Darken $color 80] } { set lc $color }\n"
"        $c create line 0 $y $w $y -fill $lc -width 1 -tag $t\n"
"        # draw db label\n"
"        set txt [expr -$l]\n"
"        if { $l == 0 } { set txt {0db} }\n"
"        set tt [$c create text $lx $y -text $txt -anchor se -justify right \\\n"
"            -font $ft -fill $txtcolor -width 0 -tags $t]\n"
"        # delete label if not enough space\n"
"        if { $l == $t0 } {\n"
"            set miny [expr 10*$zoom]\n"
"            lassign [$c bbox $tt] tx0 ty0 tx1 ty1\n"
"            if { $ty0 < $miny || $ty1 < $miny } { $c delete $tt }\n"
"        }\n"
"    }\n"
"    # top db value\n"
"    $c create text $lx 3 -text [expr int($::ui::filter::db_max)] -anchor ne -justify right \\\n"
"        -font $ft -fill $txtcolor -width 0 -tags $t\n"
"    # bottom db value\n"
"    $c create text $lx $h -text [expr int($::ui::filter::db_min)] -anchor se -justify right \\\n"
"        -font $ft -fill $txtcolor -width 0 -tags $t\n"
"}\n"
"proc complex_new {real imag} {\n"
"   return [list $real $imag]\n"
"}\n"
"proc complex_add {z1 z2} {\n"
"    set result {}\n"
"    foreach c $z1 d $z2 {\n"
"       lappend result [expr {$c+$d}]\n"
"    }\n"
"    return $result\n"
" }\n"
"proc complex_mul {z1 z2} {\n"
"   set result {}\n"
"   foreach {c1 d1} $z1 {break}\n"
"   foreach {c2 d2} $z2 {break}\n"
"   return [list [expr {$c1*$c2-$d1*$d2}] [expr {$c1*$d2+$c2*$d1}]]\n"
"}\n"
"proc complex_div {z1 z2} {\n"
"    set result {}\n"
"    foreach {c1 d1} $z1 {break}\n"
"    foreach {c2 d2} $z2 {break}\n"
"    set denom [expr {$c2*$c2+$d2*$d2}]\n"
"    return [list [expr {($c1*$c2+$d1*$d2)/$denom}] \\\n"
"                 [expr {(-$c1*$d2+$c2*$d1)/$denom}]]\n"
"}\n"
"proc complex_mod {z1} {\n"
"   foreach {c d} $z1 {break}\n"
"   return [expr {hypot($c,$d)}]\n"
"}\n"
"proc complex_exp {z1} {\n"
"   foreach {c d} $z1 {break}\n"
"   return [list [expr {exp($c)*cos($d)}] [expr {exp($c)*sin($d)}]]\n"
"}\n"
"proc filter_jw { w k } {\n"
"    set n [llength $k]\n"
"    if { $n == 0 } { return [complex_new 0.0 0.0] }\n"
"    set a [complex_new [lindex $k 0] 0.0]\n"
"    for { set j 1 } { $j < $n } { incr j } {\n"
"        set kn  [lindex $k $j]\n"
"        set jw  [complex_new 0.0 [expr -1.0*$j*$w]]\n"
"        set ejw [complex_mul [complex_new $kn 0.0] [complex_exp $jw]]\n"
"        set a   [complex_add $a $ejw]\n"
"    }\n"
"    return $a\n"
"}\n"
"proc filter_freq_amp {w b a} {\n"
"    set z [complex_new 0.0 0.0]\n"
"    set ajw [filter_jw $w $a]\n"
"    if { $ajw == $z } { return 0.0 }\n"
"    set bjw [filter_jw $w $b]\n"
"    return [complex_mod [complex_div $bjw $ajw]]\n"
"}\n"
"proc filter_x_to_omega { x w scale } {\n"
"    set f 0\n"
"    if { $scale == \"lin\" } {\n"
"        set k [expr $::ui::filter::freq_max/$::ui::filter::freq_nyq]\n"
"        set f [expr ($k*$x)/$w]\n"
"    } elseif { $scale == \"log\" } {\n"
"        set loga $filter::fmin_log10\n"
"        set logb $filter::fmax_log10\n"
"        set lrng [expr $logb-$loga]\n"
"        set px  [expr (($lrng*$x)/$w) + $loga]\n"
"        set f   [expr pow(10, $px)/$::ui::filter::freq_nyq]\n"
"    }\n"
"    expr 3.141592653589793 * $f\n"
"}\n"
"proc filter_x_to_herz { x w scale } {\n"
"    set f 0\n"
"    if { $scale == \"lin\" } {\n"
"        # lin range from 0 to $freq_max\n"
"        set x [filter::clip $x 0 $w]\n"
"        set f [expr $filter::freq_max * $x / $w]\n"
"    } elseif { $scale == \"log\" } {\n"
"        # log range from $freq_min to $freq_max\n"
"        set loga $filter::fmin_log10\n"
"        set logb $filter::fmax_log10\n"
"        set lrng [expr $logb-$loga]\n"
"        set x [filter::clip $x 0 $w]\n"
"        set px [expr (1.0*$x/$w * $lrng) + $loga]\n"
"        set f  [expr pow(10, $px)]\n"
"    }\n"
"    return $f\n"
"}\n"
"proc filter_draw_fresp { c t w h zoom color b0 b1 b2 a1 a2 scale } {\n"
"    set db_hstep [expr $h / $ui::filter::db_range]\n"
"    set pts {}\n"
"    for { set x 0 } { $x < $w } { incr x } {\n"
"        set omega [filter_x_to_omega $x $w $scale]\n"
"        set wamp  [filter_freq_amp $omega [list $b0 $b1 $b2] [list 1 $a1 $a2]]\n"
"        set dbamp [expr 20 * log10($wamp)]\n"
"        set y [expr $h - (($dbamp * $db_hstep) + ($h*0.5))]\n"
"        if { ![filter::finite $y] } { set y [expr $w+10] }\n"
"        lappend pts $x $y\n"
"    }\n"
"    $c create line $pts -fill $color -width $zoom -tags $t\n"
"}\n"
"proc filter_draw_hgrid { c t w h zoom gridcolor txtcolor } {\n"
"    set min_line_space 20\n"
"    set n_long  [llength $::ui::filter::db_labels_long]\n"
"    set n_short [llength $::ui::filter::db_labels_short]\n"
"    set space_long  [expr $h/$n_long]\n"
"    set space_short [expr $h/$n_short]\n"
"    if { $space_long >= $min_line_space } {\n"
"        filter_draw_hdb $c $t $w $h $zoom $::ui::filter::db_labels_long $gridcolor $txtcolor\n"
"    } else {\n"
"        filter_draw_hdb $c $t $w $h $zoom $::ui::filter::db_labels_short $gridcolor $txtcolor\n"
"    }\n"
"}\n"
"proc filter_draw_vgrid { c t w h zoom gridcolor txtcolor scale } {\n"
"    if { $scale == \"lin\" } {\n"
"        set min_hstep [expr 30 * $zoom]\n"
"        set fstep 5000\n"
"        set hstep [expr $fstep / $::ui::filter::freq_max * $w]\n"
"        # fine best freq step\n"
"        foreach f $::ui::filter::freq_steps {\n"
"            # hstep in pixels\n"
"            set hstep [expr ($f / $::ui::filter::freq_max) * $w]\n"
"            if { $hstep > $min_hstep } {\n"
"                set fstep $f\n"
"                break\n"
"            }\n"
"        }\n"
"        set nl [expr ($w/$hstep)]\n"
"        set ft [filter_font $zoom]\n"
"        for { set i 1 } { $i < $nl } { incr i } {\n"
"            set x [expr $i * $hstep]\n"
"            $c create line $x 0 $x $h -width 1 -fill $gridcolor -tags $t\n"
"            set f [expr $i*$fstep]\n"
"            if { $f < 1000 } { set txt \"[expr int($f)]\" } { set txt \"[expr $f/1000.0]k\" }\n"
"            set tx [expr $x-(2*$zoom+1)]\n"
"            $c create text $tx $h -text $txt -anchor se -justify right \\\n"
"                -font $ft -fill $txtcolor -width 0 -tags $t\n"
"        }\n"
"    } elseif { $scale == \"log\" } {\n"
"        set loga [expr log10($::ui::filter::freq_min)]\n"
"        set logb [expr log10($::ui::filter::freq_max)]\n"
"        set lrng [expr $logb-$loga]\n"
"        set ft [filter_font $zoom]\n"
"        set txt_right -1\n"
"        set lcolor0 [::tk::Darken $gridcolor 80]\n"
"        set lcolor1 [::tk::Darken $gridcolor 120]\n"
"        for { set i [expr int($loga)] } { $i <= $logb } { incr i } {\n"
"            set f0 [expr pow(10, $i)]\n"
"            set draw_txt 1\n"
"            set line_prev_x -1\n"
"            for { set k 0 } { $k < 9 } { incr k } {\n"
"                set f [expr $f0*(1+$k)]\n"
"                set x [expr (log10($f)-$loga)/$lrng*$w]\n"
"                # too close lines, skipping\n"
"                set too_close [expr $x-$line_prev_x < (2*$zoom)]\n"
"                if { $too_close && $k != 0 } { continue }\n"
"                set line_prev_x $x\n"
"                # 10^x freq highlight\n"
"                if { $k == 0 } { set lc $lcolor0 } { set lc $lcolor1 }\n"
"                $c create line $x 0 $x $h -width 1 -fill $lc -tags $t\n"
"                # not enough space to freq label\n"
"                if { $x < $txt_right } { $c delete $tid }\n"
"                # draw label\n"
"                if { $f < 1000 } { set txt \"[expr int($f)]\" } { set txt \"[expr int($f/1000.0)]k\" }\n"
"                set tx [expr $x+(2*$zoom)]\n"
"                set tid [$c create text $tx $h -text $txt -anchor sw -justify left \\\n"
"                    -font $ft -fill $txtcolor -width 0 -tags $t]\n"
"                lassign [$c bbox $tid] tx0 ty0 tx1 ty1\n"
"                set txt_right $tx1\n"
"            }\n"
"        }\n"
"    } elseif { $scale == \"rad\" } {\n"
"        set ft [filter_font $zoom]\n"
"        set labels { \"π/4\" \"π/2\" \"3π/4\" }\n"
"        set nl [llength $labels]\n"
"        for { set i 0 } { $i < $nl } { incr i } {\n"
"            set x [expr ($i+1.0)/($nl+1.0)*$w]\n"
"            $c create line $x 0 $x $h -width 1 -fill $gridcolor -tags $t\n"
"            set txt [lindex $labels $i]\n"
"            set tx [expr $x-(2*$zoom+1)]\n"
"            $c create text $tx $h -text $txt -anchor se -justify right \\\n"
"                -font $ft -fill $txtcolor -width 0 -tags $t\n"
"        }\n"
"    }\n"
"}\n"
"proc filter_info_txt { freq q bw type } {\n"
"    set freq [expr int($freq)]\n"
"    if { $type == \"notch\" || $type == \"peak\" || $type == \"bpf\" || $type == \"bpfq\" } {\n"
"        return \"f=${freq}Hz Q=$q bw=$bw\"\n"
"    } else {\n"
"        return \"f=${freq}Hz\"\n"
"    }\n"
"}\n"
"proc filter_draw_handle { c t w h zoom color txtcolor bdcolor bgcolor scale x y q bw type } {\n"
"    # draw Q and bandwidth label\n"
"    set pad [expr 2*$zoom]\n"
"    set tx  [expr $w/2]\n"
"    set ty  [expr $pad]\n"
"    set ft [filter_font $zoom]\n"
"    set info [filter_info_txt [filter_x_to_herz $x $w $scale] $q $bw $type]\n"
"    set tid [$c create text $tx $ty -text $info -anchor n -justify center \\\n"
"        -font $ft -fill $txtcolor -width 0 -tags $t]\n"
"    # calc bbox for bg rectangle\n"
"    lassign [$c bbox $tid] tx0 ty0 tx1 ty1\n"
"    set rx0 [expr $tx0-$pad]\n"
"    set rx1 [expr $tx1+$pad]\n"
"    set ry0 -1\n"
"    set ry1 [expr $ty1+$pad-1]\n"
"    # draw label bg rectangle\n"
"    set rid [$c create rectangle $rx0 $ry0 $rx1 $ry1 \\\n"
"        -outline $bdcolor -fill $bgcolor -width 1 -tags $t]\n"
"    $c raise $tid $rid\n"
"    # draw handle\n"
"    set r  [expr 5*$zoom]\n"
"    set x [filter::clip $x 0 $w]\n"
"    set y [filter::clip $y 0 $h]\n"
"    set x0 [expr $x-$r]\n"
"    set y0 [expr $y-$r]\n"
"    set x1 [expr $x+$r]\n"
"    set y1 [expr $y+$r]\n"
"    set cc [::tk::Darken $color 75]\n"
"    $c create oval $x0 $y0 $x1 $y1 -fill $color -outline $cc -width 1 -tags $t\n"
"}\n"
"proc filter_update { cnv id w h zoom bgcolor gridcolor txtcolor plotcolor knobcolor b0 b1 b2 a1 a2 x y scale type q bw } {\n"
"    set c [::ceammc::ui::widget_canvas $cnv $id]\n"
"    set t [::ceammc::ui::widget_tag $id]\n"
"    $c delete $t\n"
"    filter_draw_hgrid $c $t $w $h $zoom $gridcolor $txtcolor\n"
"    filter_draw_vgrid $c $t $w $h $zoom $gridcolor $txtcolor $scale\n"
"    filter_draw_fresp $c $t $w $h $zoom $plotcolor $b0 $b1 $b2 $a1 $a2 $scale\n"
"    filter_draw_handle $c $t $w $h $zoom $knobcolor $txtcolor $gridcolor $bgcolor $scale $x $y $q $bw $type\n"
"}\n"
"}\n"
;
#endif
// clang-format on
