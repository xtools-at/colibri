# Colibri Changelog

(`*` = new feature; `+` = improvement; `#` = bugfix; `~` = chore)

## v0.0.2

2024-12-19

- `*` Interfaces: finished communication via BLE
- `*` Wallet: added optional self-destruct mode after n failed login attempts
- `*` Wallet: added option to overwrite existing mnemonics
- `*` Wallet: added possibility to change password and re-encrypt existing mnemonics
- `+` Debug: added "super sensitive" log category
- `+` RPC: various improvements for methods, descriptions, params and return values
- `+` Storage: added support for custom NVS partition
- `~` Base Typescript monorepo setup for SDK- and Webapp-development

## v0.0.1

2024-12-16

Initial preview release, including the following features:

- build and flash the firmware with Arduino IDE (tested with ESP32-C3 & -S3)
- communicate with wallet via the (insecure) debug serial interface
- set a password to encrypt wallet storage
- generate truly random mnemonics and add existing ones
- store up to 30 encrypted mnemonics and switch between them
- use any HD path, and BIP32 passphrases
- sign Ethereum messages
- sign Ethereum typed data
