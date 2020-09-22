# Mac OS X Keylogger

This code is a copy and modification of this repo: https://github.com/caseyscarborough/keylogger
As an experiment to see what keys I most frequently type, I copied the above repo and modified it for my specific usage.

## Usage

```bash
$ export SDKROOT="/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk"
$ make
```

The application by default logs to `./keystroke.log` and `./words.log`

```bash
$ keylogger
Logging to: keystroke.log and words.log
```
