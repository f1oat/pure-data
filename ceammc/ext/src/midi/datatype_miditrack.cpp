#include "datatype_miditrack.h"
#include "ceammc_datastorage.h"
#include "ceammc_datatypes.h"

#include "MidiEventList.h"

const int DataTypeMidiTrack::dataType = DataStorage::instance().registerNewType("MidiTrack");

DataTypeMidiTrack::DataTypeMidiTrack()
    : events_(new MidiEventList())
{
}

DataTypeMidiTrack::DataTypeMidiTrack(const DataTypeMidiTrack& mt)
    : events_(new MidiEventList(*mt.events_))
{
}

// for std::unique_ptr
DataTypeMidiTrack::~DataTypeMidiTrack() = default;

DataTypeMidiTrack::DataTypeMidiTrack(const MidiEventList& lst)
    : events_(new MidiEventList(lst))
{
    events_->linkNotePairs();
}

DataTypeMidiTrack& DataTypeMidiTrack::operator=(const DataTypeMidiTrack& mt)
{
    if (this == &mt)
        return *this;

    events_.reset(new MidiEventList(*mt.events_));
    return *this;
}

DataTypeMidiTrack* DataTypeMidiTrack::clone() const
{
    return new DataTypeMidiTrack(*this);
}

int DataTypeMidiTrack::type() const noexcept
{
    return dataType;
}

size_t DataTypeMidiTrack::eventCount() const
{
    return events_->getSize();
}

MidiEventList& DataTypeMidiTrack::events()
{
    return *events_;
}

MidiEvent* DataTypeMidiTrack::eventAt(size_t n)
{
    return &events_->operator[](n);
}

const MidiEvent* DataTypeMidiTrack::eventAt(size_t n) const
{
    return &events_->operator[](n);
}

DataTypeMidiTrack::iterator DataTypeMidiTrack::begin()
{
    return events_->begin();
}

DataTypeMidiTrack::iterator DataTypeMidiTrack::end()
{
    return events_->end();
}

DataTypeMidiTrack::const_iterator DataTypeMidiTrack::begin() const
{
    return events_->begin();
}

DataTypeMidiTrack::const_iterator DataTypeMidiTrack::end() const
{
    return events_->end();
}
