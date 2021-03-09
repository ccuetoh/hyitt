# Hear Youtube in the Terminal -  HYITT
HYITT is a minimalistic application that allows you to stream Youtube audio directly to the console. Since just the audio and not the video gets streamed, it requires less network bandwidth. Perfect for low-connectivity or online gaming (less lag!).

The usage is pretty straightforward. Just call the executable passing a Youtube URL as the argument:
```bash
hyitt https://www.youtube.com/watch?v=LDU_Txk06tM
```
Optionally one of `--low`, `--medium`, or `--high` can be used to specify the preferred stream quality.