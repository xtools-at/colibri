export default {
  index: {
    type: 'page',
    display: 'hidden',
    theme: {
      breadcrumb: false,
      layout: 'full',
      pagination: false,
      sidebar: false,
      timestamp: false,
      toc: false,
    },
  },
  about: {
    type: 'page',
    title: 'About',
    items: {
      index: '',
      roadmap: '',
      changelog: '',
    },
  },
  build: {
    type: 'page',
    title: 'Build',
    items: {
      index: '',
      hardware: '',
      'arduino-ide': {
        title: 'Build with Arduino IDE',
        items: {
          index: 'Intro',
          'setup-ide': '',
          'flash-firmware': '',
        },
      },
    },
  },
  use: {
    type: 'page',
    title: 'Use',
    items: {
      index: '',
      setup: '',
    },
  },
  develop: {
    type: 'page',
    title: 'Develop',
    items: {
      index: '',
      'rpc-docs': '',
    },
  },
}
