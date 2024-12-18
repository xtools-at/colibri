# Colibri Wallet

<p align="center">
    <img src="https://raw.githubusercontent.com/xtools-at/colibri/main/.github/images/banner.png?sanitize=true"
        width="860" alt="Colibri - DIY crypto hardware wallet" />
</p>
<p align="center">
    <img alt="version: 0.0.X (pre-alpha)" src="https://img.shields.io/badge/version-0.0.X_(pre--alpha)-crimson" />
    <a href="https://matrix.to/#/#colibriwallet:matrix.org">
      <img alt="chat on Matrix.org" src="https://img.shields.io/badge/matrix-%23colibriwallet%3Amatrix.org-lightgreen?logo=element" />
    </a>
    <a href="https://t.me/+1vRfa1R5kUViYzM0">
      <img alt="chat on Telegram" src="https://img.shields.io/badge/chat-on_Telegram-cornflowerblue?logo=telegram" />
    </a>
    <a href="https://bsky.app/profile/xtools.at">
      <img alt="connect on Bluesky" src="https://img.shields.io/badge/%F0%9F%A6%8Bconnect-on_Bluesky-dodgerblue" />
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

**0.0.2** - `0.0.X` pre-alpha, public preview - find the full **changelog** [here](https://github.com/xtools-at/colibri/blob/main/CHANGELOG.md).

### What you can do:

- build and flash the firmware with Arduino IDE (ESP32-C3 & -S3)
- communicate with wallet via (insecure) debug serial interface (JSON-RPC)
- set a password to encrypt wallet storage
- generate truly random mnemonics and add existing ones
- store up to 30 encrypted mnemonics and switch between them
- use any HD path, and BIP32 passphrases
- sign Ethereum messages
- sign Ethereum typed data

### What you _can't_ do yet:

- use the BLE interface
- sign Ethereum transactions

---

## Build your own

Colibri doesn't require prior knowledge in hardware, or complicated tooling. It can be built with Arduino IDE only, a beginner-friendly environment to program microcontrollers. Anyone willing to bear with some initial setup and following step-by-step tutorials should be able to create their own hardware wallet quickly.

### Supported hardware

Colibri is built to support a wide range of **ESP32** chips and boards, not just specific hardware configurations. You can get these boards really cheap on [Aliexpress](https://www.aliexpress.com/w/wholesale-esp32-c3.html) (e.g. the [Supermini board](https://aliexpress.com/item/1005006406538478.html) we use in development), or on Amazon for a bit more money.

The following _Espressif_ chips are supported:

- ESP32-C3
- ESP32-S3
- ESP32 ("classic")

The minimum requirements for your development board are:

- at least one _programmable_ button (RST/EN is not enough)
- one _programmable_ LED (simple, or Neopixel RGB)
- Bluetooth Low Energy support (no `ESP32-S2` support yet)

### One-time setup of Arduino IDE

- [Download Arduino IDE](https://www.arduino.cc/en/software) and [enable support for ESP32 boards](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions)
- Go to `File > Preferences` and enable _"Show files inside sketches"_ and _"Show verbose output during compile"_
- Connect your ESP32 dev board via USB
- Change the target board in Arduino IDE (`Tools > Board > esp32 > ESP32XX Dev Module`, where "XX" is your ESP32 chip type; or select whatever fits your hardware better) and set the port (`Tools > Port`)
- Try uploading the default `Blink` sketch to your board to verify everything is working as it should (`File > Examples > 01. Basic > Blink`)
- [Try these](https://randomnerdtutorials.com/esp32-troubleshooting-guide) and [these tips](https://docs.arduino.cc/learn/starting-guide/troubleshooting-sketches/) in case you're running into any connection issues. Most common issues:

  - Wrong port selected in `Tools > Port`
  - Some boards require you to put them in flash mode manually (hold _BOOT_ button, then press _RST/EN_ button)
  - Missing drivers on Windows/macOS
  - Required user group permission on Linux

### Build firmware and flash Colibri

- Clone or download this repository
- Copy or symlink the contents of `firmware/lib` to `<user>/Documents/Arduino/libraries` (Windows, macOS) or `<user>/Arduino/libraries` (Linux)

  - In case you have some of the libraries installed already, please delete them and use the exact version from the repo
  - Do _not_ update these libraries if prompted by Arduino IDE

- Open `firmware/colibri/colibri.ino` in Arduino IDE
- Navigate to the tab `config_board.h`

  - Either pick one of the preset board-configuration by uncommenting it (more boards coming soon!)
  - Or set your own board configuration below (configure at least the pins for `LED_GPIO` and `BUTTON_GPIO_OK`)

- _Only_ if you want to use the (insecure) **debugging** features (logs, serial interface), set the following in Arduino IDE. Make sure they're **not** set for your production builds:

  - `Tools > USB CDC On Boot: Enabled`
  - `Tools > Core Debug Level: Debug`

- Make sure your board is connected and the right board type and port are selected in Arduino IDE
- Click on "Upload" to build and flash Colibri

---

## How to use

Colibri uses a modular structure to make it easy to add new hardware interfaces (e.g. BLE). At their core, all interfaces share the same JSON-RPC commands.

No matter which interface you're using to connect to your device, you can start out with this command to get an overview of all available methods and their parameters:

```json
{ "method": "listMethods" }
```

### Interfaces

Every interface has slightly different requirements to establish a connection.

#### Debug Serial Interface

If you've enabled **debugging** features for your build, the serial interface should automatically connect to your host PC, and you should be able to use Arduino IDE's _Serial Monitor_ to send commands and receive responses (if not, dis- and reconnect the board and/or restart Arduino IDE).

### Button actions

Some actions require confirmation on the hardware, you'll notice the led start flashing. If your device has two buttons, press _Ok_ to approve or _Cancel_ to reject. If you only have one, _double-click_ to approve or _long-press_ to cancel.

Additionally the following actions can be triggered on the device directly:

- Hold down _Cancel_ to lock your idle wallet. If it's locked already, it will reboot the device instead.
- Quickly press _Cancel_ ten times to trigger a complete wipe of the device.

### Wallet setup

First we need to set a _password_ for our wallet. This is used to encrypt **all** sensitive information on the device, so choosing a secure password is crucial for the security of your device. We recommend using a [Diceware](https://diceware.dmuth.org/) password with at least 3-4 random words.

```json
{ "method": "setPassword", "params": ["CorrectHorseBatteryStaple"] }
```

Next we need to generate or add a mnemonic seed phrase to our wallet. Use a **metal seed storage** to back up your seed phrase - we recommend [SAFU Ninja](https://jlopp.github.io/metal-bitcoin-storage-reviews/reviews/safu-ninja/) as a cheap and reliable DIY solution.

```javascript
// generate a 24-word mnemonic
{ "method": "createMnemonic" }

// generate a 12-word mnemonic
{ "method": "createMnemonic", "params": [12] }

// add a 12/18/24 word mnemonic:
{ "method": "addMnemonic", "params": ["your seed phrase goes here"] }
```

All done! Next time we want to use our wallet, we'll need to unlock it first:

```json
{ "method": "unlock", "params": ["CorrectHorseBatteryStaple"] }
```

To select a specific wallet, you can use

```javascript
// use your second stored mnemonic for signing, with default settings:
{ "method": "selectWallet", "params": [2] }
// this call also returns additional info for the wallet, like the address, pubkey and HD path.

// ...or be more specific:
{ "method": "selectWallet", "params": [1, "m/44'/60'/0'/0/2", "optionalPassphrase"] }
// params:
// - id of seed phrase to use (here: 1)
// - optional wallet HD path (here: Ethereum, wallet index 2)
// - optional BIP32 passphrase
```

To e.g. sign a personal message, try

```json
{ "method": "eth_signMessage", "params": ["Hello world!"] }
```

(Full JSON-RPC docs coming soon!)

---

## Development roadmap

`0.1.x`:

- Core firmware for a variety of boards
- Secure keystore
- JSON-RPC core interface
- BLE interface
- Ethereum signing
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

- Encrypted USB interface (WebUSB and/or HID)
- Bitcoin signing
- Python SDK
- Airgapped interface (camera + QR codes)
- Platform.io build setup
- Secure Boot

## Help wanted!

We're looking for contributors and sponsors to help bootstrap Colibri and make it a viable option for everyday crypto usage.
Valuable extensions to the project, once the core product is functional:

- App to setup/manage/update your wallet
- Support for more coins and chains (e.g. Bitcoin, Solana, Monero)
- 3rd party wallet integrations (e.g. MetaMask, Bitcoin HWI)
- Libraries and SDKs for builders (e.g. Typescript, Python)
- New hardware interfaces (e.g. USB support, airgap via camera/QR codes)
- Support channels for builders, users and the community
- Learning materials, content of all kinds

## Credits

- created by [xtools-at](https://github.com/xtools-at) (© 2024)
- inspired by [ricmoo](https://github.com/ricmoo)'s [Firefly](https://github.com/firefly/wallet) Arduino Wallet
- preliminary work: [ESP-5791](https://github.com/xtools-at/esp5791) - Physical backed tokens with ESP32

### Open-source libraries & Co.

- [Trezor crypto lib](https://github.com/trezor/trezor-firmware/tree/29e03bd873977a498dbce79616bfb3fe4b7a0698/crypto) ([MIT](https://github.com/xtools-at/colibri/blob/main/firmware/lib/TrezorCrypto-Arduino/src/LICENSE))
- [Trezor firmware](https://github.com/trezor/trezor-firmware/tree/29e03bd873977a498dbce79616bfb3fe4b7a0698) ([GPL-3.0](https://github.com/xtools-at/colibri/blob/main/firmware/colibri/src/chains/GPL-3.0.txt))
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson) ([MIT](https://github.com/xtools-at/colibri/blob/main/firmware/lib/ArduinoJson/LICENSE.txt))
- [NimBLE-Arduino](https://github.com/h2zero/NimBLE-Arduino) ([Apache-2.0](https://github.com/xtools-at/colibri/blob/main/firmware/lib/NimBLE-Arduino/LICENSE))

### Build setup

- Arduino IDE 2.3.2 (w/ Arduino CLI 0.35.3)
- Espressif ESP32 Arduino Core `esp32` 3.0.7 (w/ ESP-IDF 5.1)

## License

Colibri is licensed under the [AGPL 3.0](https://github.com/xtools-at/colibri/blob/main/LICENSE.md) - GNU Affero General Public License

### Disclaimer

Colibri is still under development and hasn't been peer-reviewed, or audited for security yet. Use at your own risk, and do _not_ use your real keys (yet).
_The software is provided “as-is,” without warranty of any kind, express or implied, including but not limited to the warranties of merchantability, fitness for a particular purpose, and noninfringement. In no event shall the authors or copyright holders be liable for any claim, damages, or other liability, whether in an action of contract, tort, or otherwise, arising from, out of, or in connection with the software or the use or other dealings in the software._
