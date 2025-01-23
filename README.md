# Colibri Wallet

<p align="center">
    <img src="https://raw.githubusercontent.com/xtools-at/colibri/main/.github/images/banner.png?sanitize=true"
        width="860" alt="Colibri - DIY crypto hardware wallet" />
</p>
<p align="center">
    <img alt="version: 0.0.X (pre-alpha)" src="https://img.shields.io/badge/version-0.0.X_(pre--alpha)-crimson" />
    <a href="https://matrix.to/#/#colibriwallet:matrix.org" target="_blank">
      <img alt="chat on Matrix.org" src="https://img.shields.io/badge/matrix-%23colibriwallet%3Amatrix.org-lightgreen?logo=element" />
    </a>
    <a href="https://programming.dev/c/colibri" target="_blank">
      <img alt="join Lemmy community" src="https://img.shields.io/badge/lemmy-!colibri%40programming.dev-purple?logo=lemmy" />
    </a>
    <a href="https://t.me/+1vRfa1R5kUViYzM0" target="_blank">
      <img alt="chat on Telegram" src="https://img.shields.io/badge/chat-on_Telegram-cornflowerblue?logo=telegram" />
    </a>
    <a href="https://bsky.app/profile/xtools.at" target="_blank">
      <img alt="connect on Bluesky" src="https://img.shields.io/badge/connect-on_Bluesky-dodgerblue?logo=bluesky" />
    </a>
</p>

**Colibri** is a free and open source DIY project, with the goal to enable everyone with _$5 and access to a laptop and Internet_ to create their own secure _hardware wallet_, using off-the-shelf hardware development boards.

The project aims to achieve this by

- using free, beginner-friendly tools and configuration options: everything can be built with Arduino IDE only
- utilizing cheap and widely available ESP32 development boards: one can get them for as little as ~$3
- building by the principle that "unhackable" hardware doesn't exist: any valuable data is encrypted to make it functionally impossible to extract instead

_Version `0.1.X`_ is targeted to release early 2025, and will focus on getting the Colibri core ready - creating a secure hardware keystore and communication interface, and adding support for most Ethereum-compatible networks.
It's supposed to run on most common ESP32 development boards, with a intuitively designed UI and reasonable security despite the lack of a display.

_Version `0.2.X`_ will introduce support for a wide range of display types commonly used by makers and pre-built into dev kits. This will also improve security, allowing users to verify transaction parameters, typing passwords on the device directly and recover stored mnemonics.

## Current version

**0.0.4** - `0.0.X` pre-alpha, public preview - see also [changelog](https://github.com/xtools-at/colibri/blob/main/CHANGELOG.md).

### Features:

- build and flash the firmware with Arduino IDE (tested with ESP32-C3 & -S3)
- communicate with wallet via the BLE interface (JSON-RPC)
- use the (insecure) debug serial interface
- set a password to encrypt wallet storage
- generate truly random mnemonics and add existing ones
- store up to 30 encrypted mnemonics and switch between them
- use any HD path, and BIP32 passphrases
- sign Ethereum messages
- sign Ethereum typed data
- sign Ethereum transactions

### What you _can't_ do yet:

- wallet setup and unlock via companion webapp
- use Colibri with 3rd party wallets

## How to build and use Colibri

Check out the [README in the firmware folder](https://github.com/xtools-at/colibri/blob/main/firmware/README.md) to learn which hardware you need to build your own Colibri device, how to build and flash the firmware, and setup and use your wallet.

## Development roadmap

`0.1.x`:

- [x] Core firmware with minimal UI for a variety of boards
- [x] Secure keystore
- [x] JSON-RPC core interface
- [x] BLE interface
- [x] Ethereum signing
- Trusted companion webapp for wallet setup
- Proof of concept 3rd-party wallet integration

`0.2.x`:

- Display integration and GUI
  - Show transaction data on device
  - Recover seed phrase on device
  - Enter password on device
- More presets for common dev-boards
- Typescript SDK
- 3rd-party wallet integrations

`to be prioritized`:

- USB interface (HID)
- Bitcoin signing
- Airgapped interface (camera + QR codes)
- Python SDK
- Arduino CLI build setup
- Platform.io build setup
- ESP32 Secure Boot

## Help wanted!

We're looking for contributors and sponsors to help bootstrap Colibri and make it a viable option for everyday crypto usage.
Valuable extensions to the project, once the core product is functional:

- Support for more coins and chains (e.g. Monero, Solana, Polkadot)
- 3rd party wallet integrations (e.g. MetaMask, Bitcoin HWI)
- Libraries and SDKs for builders (e.g. Typescript, Python)
- New hardware interfaces (e.g. USB support, airgap via camera/QR codes)
- Support for more advanced build environments (e.g. Platform.io, ESP-IDF, Arduino-CLI)
- Testing & QA, bugfixing and automated tests
- Support channels for builders, users and the community
- Learning materials, content of all kinds

## License

Unless otherwise noted (e.g. in README/LICENSE files, or file headers and -comments), the contents of the Colibri monorepo are _copyright © 2024-2025 by Colibri contributors_, and are licensed under the [AGPL 3.0](https://github.com/xtools-at/colibri/blob/main/LICENSE.md) - GNU Affero General Public License.

The names "Colibri", "Colibri.diy" and "Colibri Wallet", as well as the Colibri logo are copyright © 2024 by [xtools-at](https://github.com/xtools-at), and are licensed under [CC-BY-NC-4.0](https://creativecommons.org/licenses/by-nc/4.0).

The Colibri firmware additionally includes the following open-source libraries:

- [Trezor crypto lib](https://github.com/trezor/trezor-firmware/tree/29e03bd873977a498dbce79616bfb3fe4b7a0698/crypto) ([MIT](https://github.com/xtools-at/colibri/blob/main/firmware/lib/TrezorCrypto-Arduino/src/LICENSE))
- [Trezor firmware](https://github.com/trezor/trezor-firmware/tree/29e03bd873977a498dbce79616bfb3fe4b7a0698) ([GPL-3.0](https://github.com/xtools-at/colibri/blob/main/firmware/colibri/licenses/GPL-3.0.txt))
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson) ([MIT](https://github.com/xtools-at/colibri/blob/main/firmware/lib/ArduinoJson/LICENSE.txt))
- [NimBLE-Arduino](https://github.com/h2zero/NimBLE-Arduino) ([Apache-2.0](https://github.com/xtools-at/colibri/blob/main/firmware/lib/NimBLE-Arduino/LICENSE))

### Disclaimer

Colibri is still under development and hasn't been peer-reviewed, or audited for security yet. Use at your own risk, and do _not_ use your real keys (yet).
_The software is provided “as-is,” without warranty of any kind, express or implied, including but not limited to the warranties of merchantability, fitness for a particular purpose, and noninfringement. In no event shall the authors or copyright holders be liable for any claim, damages, or other liability, whether in an action of contract, tort, or otherwise, arising from, out of, or in connection with the software or the use or other dealings in the software._
