# Colibri Wallet

<p align="center">
    <img src="https://raw.githubusercontent.com/xtools-at/colibri/main/.github/images/banner.png?sanitize=true"
        width="400" alt="Colibri - DIY crypto hardware wallet" />
</p>
<p align="center">
    <img alt="version: early development" src="https://img.shields.io/badge/version-early_development-crimson" />

    <a href="https://matrix.to/#/#colibriwallet:matrix.org">
      <img alt="chat on matrix.org" src="https://img.shields.io/badge/matrix-%23colibriwallet%3Amatrix.org-brightgreen?logo=element" />
    </a>

    <a href="https://t.me/+1vRfa1R5kUViYzM0">
      <img alt="chat on Telegram" src="https://img.shields.io/badge/chat-telegram-cornflowerblue?logo=telegram" />
    </a>

</p>

## Warning: this repository is still work in progress, don't expect ANYTHING to work at this stage

**Colibri** is a free and open source DIY project, with the goal to enable everyone with _$5 and access to a laptop and Internet_ to create their own secure _hardware wallet_, using off-the-shelf hardware development boards.

The project aims to achieve this by

- using free, beginner-friendly tools and configuration options: everything can be built with Arduino IDE only
- utilizing cheap and widely available ESP32 development boards: one can get them for as little as ~$3
- building by the principle that "unhackable" hardware doesn't exist: any valuable data is encrypted to make it functionally impossible to extract instead

_Version 1_ is targeted to release end of 2024, and will focus on getting the Colibri core ready - creating a secure hardware keystore and communication interface, and adding support for most Ethereum-compatible networks.
It's supposed to run on most common ESP32 development boards which have at least one button, a LED and Bluetooth support; with a intuitively designed UI and reasonable security despite the lack of a display.

_Version 2_ will introduce support for Bitcoin, and a wide range of display types commonly used by makers and pre-built into dev kits. This will also improve security, allowing users to verify transaction parameters, typing passwords on the device directly and recover stored mnemonics.

## Supported hardware

Colibri is built for **ESP32** chips. The following types are supported:

- ESP32 (the _O.G._, sometimes called "classic" or "V1")
- ESP32 S3
- ESP32 C3

The minimum requirements for your development board are:

- at least one _programmable_ button (RST/EN is not enough)
- one _programmable_ LED (simple, or Neopixel RGB)
- Bluetooth Low Energy support (sorry `S2` board owners)

## Help wanted!

We're looking for contributors and sponsors to help bootstrap Colibri and make it a viable option for everyday crypto usage.
Valuable extensions to the project, once the core product is functional:

- App to setup/manage/update your wallet
- Support for more coins and chains (e.g. Dogecoin, Solana, Monero)
- 3rd party wallet integrations (e.g. MetaMask, Bitcoin HWI)
- Libraries and SDKs for builders (e.g. Typescript, Python)
- New hardware interfaces (e.g. USB support, airgap via camera/QR codes)
- Support channels for builders, users and the community
- Learning materials, content of all kinds
