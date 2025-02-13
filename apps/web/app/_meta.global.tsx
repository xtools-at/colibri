export default {
  index: {
    type: 'page',
    display: 'hidden',
  },
  build: {
    type: 'page',
    title: 'Build',
    items: {
      index: 'Build your own crypto hardware wallet',
      'get-esp32-board': '',
      'setup-arduino-ide': '',
      'flash-firmware': '',
    },
  },
  use: {
    type: 'page',
    title: 'Use',
  },
  develop: {
    type: 'page',
    title: 'Develop',
  },
  tools: {
    type: 'page',
    title: 'Tools',
    items: {
      index: '',
      'wallet-setup': '',
    },
  },
  /*
  docs: {
    type: 'page',
    title: 'Documentation',
    items: {
      index: '',
      guide: {
        items: {
          link: '',
          image: '',
          markdown: '',
        },
      },
    },
  },
  versions: {
    type: 'menu',
    title: 'Versions',
    items: {
      _3: {
        title: 'Nextra v3 Docs',
        href: 'https://nextra-v2-7hslbun8z-shud.vercel.app',
      },
      _2: {
        title: 'Nextra v2 Docs',
        href: 'https://nextra-v2-oe0zrpzjp-shud.vercel.app',
      },
    },
  },

  */
}
