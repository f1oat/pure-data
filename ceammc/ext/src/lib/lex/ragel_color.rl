%%{

machine color_rgb_hex;

action color_hex_set  { color.x = xchar2digit(fc); }
action color_hex_add  { (color.x *= 16) += xchar2digit(fc); }

color_xhex = xdigit{2} >color_hex_set $color_hex_add;

color_rgb = '#'
          (color_xhex %{color.r = color.x;}
           color_xhex %{color.g = color.x;}
           color_xhex %{color.b = color.x;})
           %{ cat_ = CAT_COLOR; type_ = TYPE_HEX; };

}%%

%%{

machine color_rgb_hex_short;

action color_hex_set  { color.x = xchar2digit(fc); }

color_xhex = xdigit >color_hex_set;

color_rgb = '#'
          (color_xhex %{color.r = color.x;}
           color_xhex %{color.g = color.x;}
           color_xhex %{color.b = color.x;})
           %{ cat_ = CAT_COLOR; type_ = TYPE_HEX; };

}%%

%%{

machine color_rgba_hex;

action color_rgba_hex_set  { color.x = xchar2digit(fc); }
action color_rgba_hex_add  { (color.x *= 16) += xchar2digit(fc); }

color_xhex = xdigit{2} >color_rgba_hex_set $color_rgba_hex_add;

color_rgba = '#'
          (color_xhex %{color.r = color.x;}
           color_xhex %{color.g = color.x;}
           color_xhex %{color.b = color.x;}
           color_xhex %{color.a = color.x;})
           %{ cat_ = CAT_COLOR; type_ = TYPE_HEX; };

}%%

%%{

machine color_rgba_hex_short;

action color_hex_set  { color.x = xchar2digit(fc); }

color_xhex = xdigit >color_hex_set;

color_rgba = '#'
          (color_xhex %{color.r = color.x;}
           color_xhex %{color.g = color.x;}
           color_xhex %{color.b = color.x;}
           color_xhex %{color.a = color.x;})
           %{ cat_ = CAT_COLOR; type_ = TYPE_HEX; };

}%%
