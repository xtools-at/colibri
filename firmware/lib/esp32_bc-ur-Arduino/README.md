# [BC-UR Library for ESP32 and Arduino IDE](https://github.com/xtools-at/colibri/tree/main/firmware/lib/esp32_bc-ur-Arduino)

Forked from Blockstream's [Blockchain Commons UR Library for ESP32 IDF](https://github.com/blockstream/esp32_bc-ur), which itself is a fork of [bc-ur](https://github.com/BlockchainCommons/bc-ur) from Blockchain Commons with some patches to run on esp32 via the [esp idf framework](https://github.com/espressif/esp-idf).

> Adapted for Arduino IDE from commit _[0c94321e1d60ce81631d022bab8cdbfa6dc6aa0d](https://github.com/Blockstream/esp32_bc-ur/tree/0c94321e1d60ce81631d022bab8cdbfa6dc6aa0d)_ for [Colibri Wallet](https://colibri.diy) by [xtools](https://github.com/xtools-at).
>
> Requires _[TinyCBOR](https://github.com/soburi/tinycbor)_ Arduino library to be installed alongside in Arduino IDE.

## Origin, Authors, Copyright & Licenses

Unless otherwise noted (either in this [/README.md](./README.md) or in the file's header comments) the contents of this repository are Copyright © 2020 by Blockchain Commons, LLC, and are [licensed](./LICENSE) under the [spdx:BSD-2-Clause Plus Patent License](https://spdx.org/licenses/BSD-2-Clause-Patent.html).

The table below establishes provenance (repository of origin, permalink, and commit id) for each source file in this repository. Contributors to these files are listed in the commit history for each file, first in this repo, then in the repo of their origin.

In most cases, the authors, copyright, and license for each file reside in comments in the source. When it does not we have attempted to attribute it accurately below.

| File                              | From                                                             | Commit | Authors & Copyright (c)                                                    | License                                                            |
| --------------------------------- | ---------------------------------------------------------------- | ------ | -------------------------------------------------------------------------- | ------------------------------------------------------------------ |
| crc32.c<br/>crc32.h               | NA                                                               | NA     | 2020 Wolf McNally for Blockchain Commons                                   | [BSD-3-Clause](https://spdx.org/licenses/BSD-3-Clause.html)        |
| xoshiro256.cpp<br/>xoshiro256.hpp | [prng.di.unimi.it](http://prng.di.unimi.it/xoshiro256starstar.c) | NA     | David Blackman and Sebastiano Vigna, with C++ enhancements by Wolf McNally | [Public Domain](http://creativecommons.org/publicdomain/zero/1.0/) |

### Credits

The following people directly contributed to this repository. You can add your name here by getting involved — the first step is to learn how to contribute from our [CONTRIBUTING.md](./CONTRIBUTING.md) documentation.

| Name              | Role                | Github                                           | Email                                 | GPG Fingerprint                                    |
| ----------------- | ------------------- | ------------------------------------------------ | ------------------------------------- | -------------------------------------------------- |
| Christopher Allen | Principal Architect | [@ChristopherA](https://github.com/ChristopherA) | \<ChristopherA@LifeWithAlacrity.com\> | FDFE 14A5 4ECB 30FC 5D22 74EF F8D3 6C91 3574 05ED  |
| Wolf McNally      | Project Lead        | [@WolfMcNally](https://github.com/wolfmcnally)   | \<Wolf@WolfMcNally.com\>              | 9436 52EE 3844 1760 C3DC  3536 4B6C 2FCF 8947 80AE |
