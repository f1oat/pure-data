
#line 1 "lex/parser_color.rl"
# include "parser_color.h"
# include "ragel_common.h"

# include <cstdint>
# include <cstring>

namespace ceammc {
namespace parser {

// for autogenerated ragel internals
struct ColorRagelData {
    int r { 0 };
    int g { 0 };
    int b { 0 };
    int a { 0 };
    int x { 0 };
};

RgbHexFullMatch::RgbHexFullMatch()
{
    reset();
}

void RgbHexFullMatch::reset()
{
    color_ = {};
}

bool RgbHexFullMatch::parse(const Atom& a)
{
    if (a.isSymbol())
        return parse(a.asT<t_symbol*>()->s_name);
    else
        return false;
}

size_t RgbHexFullMatch::parse(const AtomListView& lv, SmallColorVec& out)
{
    const size_t N = lv.size();

    for (size_t i = 0; i < N; i++) {
        const auto& a = lv[i];
        if (!parse(a))
            return i;

        out.push_back(asInt());
    }

    return N;
}

bool RgbHexFullMatch::parse(const char* str)
{
    auto len = strlen(str);
    if (len == 4)
        return parseShort(str, len);
    else if(len == 7)
        return parseFull(str, len);
    else
        return false;
}


#line 67 "lex/parser_color.cpp"
static const int color_start = 1;
static const int color_first_final = 8;
static const int color_error = 0;

static const int color_en_main = 1;


#line 69 "lex/parser_color.rl"


bool RgbHexFullMatch::parseFull(const char* str, size_t len)
{
    int cs = 0;
    const char* p = str;
    const char* pe = p + len;
    const char* eof = pe;
    ColorRagelData color;
    AtomCategory cat_ {CAT_UNKNOWN};
    AtomType type_ = {TYPE_UNKNOWN};

    reset();

    
#line 91 "lex/parser_color.cpp"
	{
	cs = color_start;
	}

#line 84 "lex/parser_color.rl"
    
#line 98 "lex/parser_color.cpp"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
case 1:
	if ( (*p) == 35 )
		goto st2;
	goto st0;
st0:
cs = 0;
	goto _out;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr2;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr2;
	} else
		goto tr2;
	goto st0;
tr2:
#line 5 "lex/ragel_color.rl"
	{ color.x = xchar2digit((*p)); }
#line 6 "lex/ragel_color.rl"
	{ (color.x *= 16) += xchar2digit((*p)); }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 134 "lex/parser_color.cpp"
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr3;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr3;
	} else
		goto tr3;
	goto st0;
tr3:
#line 6 "lex/ragel_color.rl"
	{ (color.x *= 16) += xchar2digit((*p)); }
	goto st4;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
#line 152 "lex/parser_color.cpp"
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr4;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr4;
	} else
		goto tr4;
	goto st0;
tr4:
#line 11 "lex/ragel_color.rl"
	{color.r = color.x;}
#line 5 "lex/ragel_color.rl"
	{ color.x = xchar2digit((*p)); }
#line 6 "lex/ragel_color.rl"
	{ (color.x *= 16) += xchar2digit((*p)); }
	goto st5;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
#line 174 "lex/parser_color.cpp"
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr5;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr5;
	} else
		goto tr5;
	goto st0;
tr5:
#line 6 "lex/ragel_color.rl"
	{ (color.x *= 16) += xchar2digit((*p)); }
	goto st6;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
#line 192 "lex/parser_color.cpp"
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr6;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr6;
	} else
		goto tr6;
	goto st0;
tr6:
#line 12 "lex/ragel_color.rl"
	{color.g = color.x;}
#line 5 "lex/ragel_color.rl"
	{ color.x = xchar2digit((*p)); }
#line 6 "lex/ragel_color.rl"
	{ (color.x *= 16) += xchar2digit((*p)); }
	goto st7;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
#line 214 "lex/parser_color.cpp"
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr7;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr7;
	} else
		goto tr7;
	goto st0;
tr7:
#line 6 "lex/ragel_color.rl"
	{ (color.x *= 16) += xchar2digit((*p)); }
	goto st8;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
#line 232 "lex/parser_color.cpp"
	goto st0;
	}
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 
	_test_eof8: cs = 8; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch ( cs ) {
	case 8: 
#line 13 "lex/ragel_color.rl"
	{color.b = color.x;}
#line 14 "lex/ragel_color.rl"
	{ cat_ = CAT_COLOR; type_ = TYPE_HEX; }
	break;
#line 253 "lex/parser_color.cpp"
	}
	}

	_out: {}
	}

#line 85 "lex/parser_color.rl"

    const auto ok = cs >= 8;
    if(ok) {
        color_.r = color.r;
        color_.g = color.g;
        color_.b = color.b;
    }

    return ok;
}


#line 273 "lex/parser_color.cpp"
static const int rgb_color_short_start = 1;
static const int rgb_color_short_first_final = 5;
static const int rgb_color_short_error = 0;

static const int rgb_color_short_en_main = 1;


#line 102 "lex/parser_color.rl"


bool RgbHexFullMatch::parseShort(const char* str, size_t length)
{
    int cs = 0;
    const char* p = str;
    const char* pe = p + length;
    const char* eof = pe;
    ColorRagelData color;
    AtomCategory cat_ {CAT_UNKNOWN};
    AtomType type_ = {TYPE_UNKNOWN};

    reset();

    
#line 297 "lex/parser_color.cpp"
	{
	cs = rgb_color_short_start;
	}

#line 117 "lex/parser_color.rl"
    
#line 304 "lex/parser_color.cpp"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
case 1:
	if ( (*p) == 35 )
		goto st2;
	goto st0;
st0:
cs = 0;
	goto _out;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr2;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr2;
	} else
		goto tr2;
	goto st0;
tr2:
#line 22 "lex/ragel_color.rl"
	{ color.x = xchar2digit((*p)); }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 338 "lex/parser_color.cpp"
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr3;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr3;
	} else
		goto tr3;
	goto st0;
tr3:
#line 27 "lex/ragel_color.rl"
	{color.r = color.x;}
#line 22 "lex/ragel_color.rl"
	{ color.x = xchar2digit((*p)); }
	goto st4;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
#line 358 "lex/parser_color.cpp"
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr4;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr4;
	} else
		goto tr4;
	goto st0;
tr4:
#line 28 "lex/ragel_color.rl"
	{color.g = color.x;}
#line 22 "lex/ragel_color.rl"
	{ color.x = xchar2digit((*p)); }
	goto st5;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
#line 378 "lex/parser_color.cpp"
	goto st0;
	}
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch ( cs ) {
	case 5: 
#line 29 "lex/ragel_color.rl"
	{color.b = color.x;}
#line 30 "lex/ragel_color.rl"
	{ cat_ = CAT_COLOR; type_ = TYPE_HEX; }
	break;
#line 396 "lex/parser_color.cpp"
	}
	}

	_out: {}
	}

#line 118 "lex/parser_color.rl"

    const auto ok = cs >= 5;
    if(ok) {
        color_.r = color.r | (color.r << 4);
        color_.g = color.g | (color.g << 4);
        color_.b = color.b | (color.b << 4);
    }

    return ok;

    return false;
}

RgbaHexFullMatch::RgbaHexFullMatch()
{
    reset();
}

void RgbaHexFullMatch::reset()
{
    color_ = {};
}

bool RgbaHexFullMatch::parse(const Atom& a)
{
    if (a.isSymbol())
        return parse(a.asT<t_symbol*>()->s_name);
    else
        return false;
}

bool RgbaHexFullMatch::parse(const char* str)
{
    auto len = strlen(str);
    if (len == 5)
        return parseShort(str, len);
    else if(len == 9)
        return parseFull(str, len);
    else
        return false;
}


#line 447 "lex/parser_color.cpp"
static const int rgba_color_start = 1;
static const int rgba_color_first_final = 10;
static const int rgba_color_error = 0;

static const int rgba_color_en_main = 1;


#line 166 "lex/parser_color.rl"


bool RgbaHexFullMatch::parseFull(const char* str, size_t length)
{
    int cs = 0;
    const char* p = str;
    const char* pe = p + length;
    const char* eof = pe;
    ColorRagelData color;
    AtomCategory cat_ {CAT_UNKNOWN};
    AtomType type_ = {TYPE_UNKNOWN};

    reset();

    
#line 471 "lex/parser_color.cpp"
	{
	cs = rgba_color_start;
	}

#line 181 "lex/parser_color.rl"
    
#line 478 "lex/parser_color.cpp"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
case 1:
	if ( (*p) == 35 )
		goto st2;
	goto st0;
st0:
cs = 0;
	goto _out;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr2;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr2;
	} else
		goto tr2;
	goto st0;
tr2:
#line 38 "lex/ragel_color.rl"
	{ color.x = xchar2digit((*p)); }
#line 39 "lex/ragel_color.rl"
	{ (color.x *= 16) += xchar2digit((*p)); }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 514 "lex/parser_color.cpp"
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr3;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr3;
	} else
		goto tr3;
	goto st0;
tr3:
#line 39 "lex/ragel_color.rl"
	{ (color.x *= 16) += xchar2digit((*p)); }
	goto st4;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
#line 532 "lex/parser_color.cpp"
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr4;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr4;
	} else
		goto tr4;
	goto st0;
tr4:
#line 44 "lex/ragel_color.rl"
	{color.r = color.x;}
#line 38 "lex/ragel_color.rl"
	{ color.x = xchar2digit((*p)); }
#line 39 "lex/ragel_color.rl"
	{ (color.x *= 16) += xchar2digit((*p)); }
	goto st5;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
#line 554 "lex/parser_color.cpp"
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr5;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr5;
	} else
		goto tr5;
	goto st0;
tr5:
#line 39 "lex/ragel_color.rl"
	{ (color.x *= 16) += xchar2digit((*p)); }
	goto st6;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
#line 572 "lex/parser_color.cpp"
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr6;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr6;
	} else
		goto tr6;
	goto st0;
tr6:
#line 45 "lex/ragel_color.rl"
	{color.g = color.x;}
#line 38 "lex/ragel_color.rl"
	{ color.x = xchar2digit((*p)); }
#line 39 "lex/ragel_color.rl"
	{ (color.x *= 16) += xchar2digit((*p)); }
	goto st7;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
#line 594 "lex/parser_color.cpp"
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr7;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr7;
	} else
		goto tr7;
	goto st0;
tr7:
#line 39 "lex/ragel_color.rl"
	{ (color.x *= 16) += xchar2digit((*p)); }
	goto st8;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
#line 612 "lex/parser_color.cpp"
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr8;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr8;
	} else
		goto tr8;
	goto st0;
tr8:
#line 46 "lex/ragel_color.rl"
	{color.b = color.x;}
#line 38 "lex/ragel_color.rl"
	{ color.x = xchar2digit((*p)); }
#line 39 "lex/ragel_color.rl"
	{ (color.x *= 16) += xchar2digit((*p)); }
	goto st9;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
#line 634 "lex/parser_color.cpp"
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr9;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr9;
	} else
		goto tr9;
	goto st0;
tr9:
#line 39 "lex/ragel_color.rl"
	{ (color.x *= 16) += xchar2digit((*p)); }
	goto st10;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
#line 652 "lex/parser_color.cpp"
	goto st0;
	}
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 
	_test_eof8: cs = 8; goto _test_eof; 
	_test_eof9: cs = 9; goto _test_eof; 
	_test_eof10: cs = 10; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch ( cs ) {
	case 10: 
#line 47 "lex/ragel_color.rl"
	{color.a = color.x;}
#line 48 "lex/ragel_color.rl"
	{ cat_ = CAT_COLOR; type_ = TYPE_HEX; }
	break;
#line 675 "lex/parser_color.cpp"
	}
	}

	_out: {}
	}

#line 182 "lex/parser_color.rl"

    const auto ok = cs >= 10;
    if(ok) {
        color_.r = color.r;
        color_.g = color.g;
        color_.b = color.b;
        color_.a = color.a;
    }

    return ok;
}


#line 696 "lex/parser_color.cpp"
static const int rgba_color_short_start = 1;
static const int rgba_color_short_first_final = 6;
static const int rgba_color_short_error = 0;

static const int rgba_color_short_en_main = 1;


#line 200 "lex/parser_color.rl"


bool RgbaHexFullMatch::parseShort(const char* str, size_t length)
{
    int cs = 0;
    const char* p = str;
    const char* pe = p + length;
    const char* eof = pe;
    ColorRagelData color;
    AtomCategory cat_ {CAT_UNKNOWN};
    AtomType type_ = {TYPE_UNKNOWN};

    reset();

    
#line 720 "lex/parser_color.cpp"
	{
	cs = rgba_color_short_start;
	}

#line 215 "lex/parser_color.rl"
    
#line 727 "lex/parser_color.cpp"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
case 1:
	if ( (*p) == 35 )
		goto st2;
	goto st0;
st0:
cs = 0;
	goto _out;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr2;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr2;
	} else
		goto tr2;
	goto st0;
tr2:
#line 56 "lex/ragel_color.rl"
	{ color.x = xchar2digit((*p)); }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 761 "lex/parser_color.cpp"
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr3;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr3;
	} else
		goto tr3;
	goto st0;
tr3:
#line 61 "lex/ragel_color.rl"
	{color.r = color.x;}
#line 56 "lex/ragel_color.rl"
	{ color.x = xchar2digit((*p)); }
	goto st4;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
#line 781 "lex/parser_color.cpp"
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr4;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr4;
	} else
		goto tr4;
	goto st0;
tr4:
#line 62 "lex/ragel_color.rl"
	{color.g = color.x;}
#line 56 "lex/ragel_color.rl"
	{ color.x = xchar2digit((*p)); }
	goto st5;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
#line 801 "lex/parser_color.cpp"
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr5;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr5;
	} else
		goto tr5;
	goto st0;
tr5:
#line 63 "lex/ragel_color.rl"
	{color.b = color.x;}
#line 56 "lex/ragel_color.rl"
	{ color.x = xchar2digit((*p)); }
	goto st6;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
#line 821 "lex/parser_color.cpp"
	goto st0;
	}
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch ( cs ) {
	case 6: 
#line 64 "lex/ragel_color.rl"
	{color.a = color.x;}
#line 65 "lex/ragel_color.rl"
	{ cat_ = CAT_COLOR; type_ = TYPE_HEX; }
	break;
#line 840 "lex/parser_color.cpp"
	}
	}

	_out: {}
	}

#line 216 "lex/parser_color.rl"

    const auto ok = cs >= 6;
    if(ok) {
        color_.r = color.r | (color.r << 4);
        color_.g = color.g | (color.g << 4);
        color_.b = color.b | (color.b << 4);
        color_.a = color.a | (color.a << 4);
    }

    return ok;
}

}
}


