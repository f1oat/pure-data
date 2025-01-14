#ifndef ARRAY_STRETCH_H
#define ARRAY_STRETCH_H

#include <memory>

#include "array_base.h"

class PdSoundTouch;

class ArrayStretch : public BaseObject {
    Array src_array_;
    Array dest_array_;

    t_symbol* src_array_name_;
    t_symbol* dest_array_name_;
    bool speech_;

    FloatProperty* pitch_;
    FloatProperty* tempo_;
    FloatProperty* rate_;

    std::shared_ptr<PdSoundTouch> soundtouch_;

public:
    ArrayStretch(const PdArgs& a);
    void onBang() override;

    bool setSrcArray(t_symbol* s);
    bool setDestArray(t_symbol* s);

    void propSetSeekWindow(const AtomList& ms);

private:
    void setupSoundTouch();
};

void setup_array_stretch();

#endif // ARRAY_STRETCH_H
