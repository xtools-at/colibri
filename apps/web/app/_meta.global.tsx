export default {
  index: {
    type: 'page',
    display: 'hidden',
  },
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
  /*
  about: {
    type: 'page',
    theme: {
      typesetting: 'article',
      // layout: 'full',
      // timestamp: false,
    },
  },
  */
}
