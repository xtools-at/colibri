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
- Bluetooth Low Energy support

#### Experimental hardware support

```bash
$ echo "Developers only!"
```

- _ESP32-C6_ and _ESP32-H2_ may work in theory, but haven't been tested yet.
- _ESP32-S2_ and _ESP32-P4_ don't support BLE, but have USB-OTG. They could be used for development purposes with the _Serial Debug Interface_.

### One-time setup of Arduino IDE

- [Download Arduino IDE](https://www.arduino.cc/en/software) and [enable support for ESP32 boards](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions). Internally we're using these versions to build:
  - Arduino IDE 2.3.2 (w/ Arduino CLI 0.35.3)
  - `esp32` Arduino Core 3.1.0 (w/ ESP-IDF 5.3)
- Go to `File > Preferences` and enable _"Show files inside sketches"_ and _"Show verbose output during compile"_
- Connect your ESP32 dev board via USB
- Change the target board in Arduino IDE (`Tools > Board > esp32 > ESP32XX Dev Module`, where "XX" is your ESP32 chip type; or select whatever fits your hardware better)
- Set the USB port your board is connected to in `Tools > Port`
- Try uploading the default `Blink` sketch to your board to verify everything is working as it should (`File > Examples > 01. Basic > Blink`)
- [Try these](https://randomnerdtutorials.com/esp32-troubleshooting-guide) and [these tips](https://docs.arduino.cc/learn/starting-guide/troubleshooting-sketches/) in case you're running into any connection issues. Most common issues:

  - Some boards require you to put them in flash mode manually (hold down _BOOT_ button, then press _RST/EN_ button)
  - [Wrong port selected](https://docs.arduino.cc/learn/starting-guide/troubleshooting-sketches/#board--port) in `Tools > Port`
  - [Missing drivers on Windows/macOS](https://randomnerdtutorials.com/esp32-troubleshooting-guide/#COM-port-not-found)
  - [Required user group permission on Linux](https://support.arduino.cc/hc/en-us/articles/360016495679-Fix-port-access-on-Linux)

### Build firmware and flash Colibri

- Clone or download the [repository](https://github.com/xtools-at/colibri)
- Copy or symlink the contents of `firmware/lib` to `<user>/Documents/Arduino/libraries` (Windows, macOS) or `<user>/Arduino/libraries` (Linux)

  - In case you have some of the libraries installed already, please delete them and use the exact version from the repo
  - Do _not_ update these libraries if prompted by Arduino IDE

- Open `firmware/colibri/colibri.ino` in Arduino IDE
- Navigate to the tab `config_board.h`

  - Either pick one of the preset board-configuration by uncommenting it (more boards coming soon!)
  - Or set your own board configuration below (configure at least the pins for `LED_GPIO` and `BUTTON_GPIO_OK`)

- Make sure your board is connected and the right _board type_ and _port_ are selected in Arduino IDE
- Click on "Upload" to build and flash Colibri

In case the compiled sketch exceeds your board's memory, try setting `Tools > Partition Scheme > Huge App`or `> Minimal SPIFFS` and recompile.

#### Debug builds

- _Developers only:_ if you want to use the (insecure) **debugging** features (logs, serial interface), set the following in Arduino IDE. Make sure they're **not** set for your production builds:

  - `Tools > USB CDC On Boot: Enabled`
  - `Tools > Core Debug Level: Debug`

---

## How to use

Colibri uses a modular structure to make it easy to add new hardware interfaces (e.g. BLE). At their core, all interfaces share the same JSON-RPC commands.

No matter which interface you're using to connect to your device, you can start out with this command to get an overview of all available methods and their parameters:

```json
{ "method": "listMethods" }
```

### Interfaces

Every interface has slightly different requirements to establish a connection.

#### Bluetooth Low Energy (BLE) Interface

To connect to your wallet via BLE, you need to pair your device first in your operating system's Bluetooth settings. When prompted for a pairing code, enter `200913` (you can change the default key in `config_custom.h`). On devices with screens, it will be randomly generated and displayed.

Now that your wallet is connected and paired, you can use an app like e.g. [nRF Connect](https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp&hl=en) to send commands to it.

The device exposes a GATT service with the UUID `31415926-5358-9793-2384-626433832795`, with two characteristics:

- `C001`: _write_ your JSON-RPC command to this field
- `C000`: you can _read_ the JSON-RPC response from this field (turn on notifications for best results)

A more user-friendly way to use the BLE interface is coming soon.

#### Debug Serial Interface

If you've enabled **debugging** features for your build, the serial interface should automatically connect to your host PC, and you should be able to use Arduino IDE's _Serial Monitor_ to send commands and receive responses (if not, dis- and reconnect the board and/or restart Arduino IDE).

### Button actions

Some actions require confirmation on the hardware, you'll notice the led start flashing. If your device has two buttons, press _Ok_ to approve or _Cancel_ to reject. If you only have one, _double-click_ to approve or _long-press_ to cancel.

Additionally the following actions can be triggered on the device directly:

- Hold down _Cancel_ to lock your idle wallet. If it's locked already, it will reboot the device instead.
- Quickly press _Cancel_ ten times to trigger a complete wipe of the device.

### Wallet setup and JSON-RPC command examples

First we need to set a _password_ for our wallet. This is used to encrypt **all** sensitive information, so choosing a secure password is crucial for the security of your device. We recommend using a [Diceware](https://diceware.dmuth.org/) password with at least 3-4 random words.

```json
{ "method": "setPassword", "params": ["CorrectHorseBatteryStaple"] }
```

Next we need to generate or add a mnemonic seed phrase to our wallet. Use a **metal seed storage** to back up your seed phrase - we recommend [SAFU Ninja](https://jlopp.github.io/metal-bitcoin-storage-reviews/reviews/safu-ninja/) as a cheap and reliable DIY solution.

```javascript
// generate a 24-word mnemonic
{ "method": "createSeedPhrase" }

// generate a 12-word mnemonic
{ "method": "createSeedPhrase", "params": [12] }

// add a 12/18/24 word mnemonic:
{ "method": "addSeedPhrase", "params": ["your seed phrase goes here"] }
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
{ "method": "signMessage", "params": ["Hello world!"] }
```

Full JSON-RPC docs coming soon.

---

## Developer quickstart

If you want to contribute to the development of Colibri, first do _all of the above_, even if only to get the hang of it :) If you don't know where to start, see the [roadmap](#development-roadmap) and ["Help wanted"](#help-wanted) section below to find ideas for some valuable extensions.

Then proceed to:

- Fork the repo and clone
- Open the repo in VSCode, install _all_ recommended extensions

For `firmware` development:

- Tweak `.vscode/c_cpp_properties.json` to match your operating system's specifics
  - Linux should work out of the box
  - Windows and macOS configs exist, but are likely wrong or outdated. Activate the right config by clicking on "Linux" in the bottom right corner while having the file above opened
- Trigger builds and uploads from Arduino IDE

For any kind of JS/TS `packages` or `apps` (e.g. SDK, Webapp):

- Install Node.js 20 (e.g. via [nvm](https://github.com/nvm-sh/nvm)), and install _pnpm_: `npm i -g pnpm`
- Install dependencies: `pnpm i`
- (more instructions are coming once there's something to build)

Finally, open a PR with your changes. Don't forget to update `README` and `CHANGELOG`, and to put yourself on the `CONTRIBUTORS` list :)

---

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

- Support for more coins and chains (e.g. Monero, Solana)
- 3rd party wallet integrations (e.g. MetaMask, Bitcoin HWI)
- Libraries and SDKs for builders (e.g. Typescript, Python)
- New hardware interfaces (e.g. USB support, airgap via camera/QR codes)
- Support for more advanced build environments (e.g. Platform.io, ESP-IDF, Arduino-CLI)
- Testing & QA, bugfixing and automated tests
- Support channels for builders, users and the community
- Learning materials, content of all kinds

## Credits

- created by [xtools-at](https://github.com/xtools-at) (© 2024-2025)
- inspired by [ricmoo](https://github.com/ricmoo)'s [Firefly](https://github.com/firefly/wallet) Arduino Wallet
- preliminary work: [ESP-5791](https://github.com/xtools-at/esp5791) - Physical backed tokens with ESP32

### Open-source libraries & Co.

- [Trezor crypto lib](https://github.com/trezor/trezor-firmware/tree/29e03bd873977a498dbce79616bfb3fe4b7a0698/crypto) ([MIT](https://github.com/xtools-at/colibri/blob/main/firmware/lib/TrezorCrypto-Arduino/src/LICENSE))
- [Trezor firmware](https://github.com/trezor/trezor-firmware/tree/29e03bd873977a498dbce79616bfb3fe4b7a0698) ([GPL-3.0](https://github.com/xtools-at/colibri/blob/main/firmware/colibri/licenses/GPL-3.0.txt))
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson) ([MIT](https://github.com/xtools-at/colibri/blob/main/firmware/lib/ArduinoJson/LICENSE.txt))
- [NimBLE-Arduino](https://github.com/h2zero/NimBLE-Arduino) ([Apache-2.0](https://github.com/xtools-at/colibri/blob/main/firmware/lib/NimBLE-Arduino/LICENSE))

## License

The Colibri monorepo and firmware/apps are licensed under the [AGPL 3.0](https://github.com/xtools-at/colibri/blob/main/LICENSE.md) - GNU Affero General Public License

### Disclaimer

Colibri is still under development and hasn't been peer-reviewed, or audited for security yet. Use at your own risk, and do _not_ use your real keys (yet).
_The software is provided “as-is,” without warranty of any kind, express or implied, including but not limited to the warranties of merchantability, fitness for a particular purpose, and noninfringement. In no event shall the authors or copyright holders be liable for any claim, damages, or other liability, whether in an action of contract, tort, or otherwise, arising from, out of, or in connection with the software or the use or other dealings in the software._
