#ifndef EXT_GORILLA_AUDIO_HPP
#define EXT_GORILLA_AUDIO_HPP

namespace ext {

class GorillaAudio final
{
public:
     GorillaAudio();
    ~GorillaAudio();

    GorillaAudio            (GorillaAudio const&) = delete;
    GorillaAudio& operator= (GorillaAudio const&) = delete;
};

} // namespace ext

#endif // EXT_GORILLA_AUDIO_HPP
