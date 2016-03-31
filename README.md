# PJON Protocol Analyzer for Saleae Logic
- What is a Saleae Logic Analyzer? An ultimate hardware hacking and debugging tool for digital MCUs which people actually love to use: https://www.saleae.com/
- What is a PJON? A single-wire protocol with built-in reliability and error correction https://github.com/gioblu/PJON

# Screenshot (click through to RAW for a full-size)
![Overview Screenshot](/assets/screenshot_overview.png)

# Quickstart
- Tell Logic to load the protocol analyzer plugin. Full details avilable in the Analyzer SDK docs PDF, but here's a quickie:
 - Launch the Logic software.
 - Select `Options->Preferences`
 - Under `Developer`, click `Browse`. Navigate to the directory you downloaded the protocol binary to
 - Restart Logic
- In Logic, go to `Options -> Open Capture` and load the example file from the `assets` folder of this repository. This is a 'hello world' of PJON sending a byte every second.
- In `Analyzers` click `+ -> Show More -> PJON`

# Compiling
Follow standard Saleae documentation, no surprises (built with 1.1.32) http://support.saleae.com/hc/en-us/articles/201104644-Protocol-Analyzer-SDK
- OSX http://support.saleae.com/hc/en-us/articles/208019393-OSX-Analyzer-SDK-Xcode-setup
