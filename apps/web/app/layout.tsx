import {
  GithubLogo,
  MatrixLogo,
  BlueskyLogo,
  LemmyLogo,
  TelegramLogo,
  MastodonLogo,
} from '@components/icons'
import cn from 'clsx'
import type { Metadata } from 'next'
import { Footer, Layout, Link, Navbar } from 'nextra-theme-docs'
import { Head, Image } from 'nextra/components'
import { getPageMap } from 'nextra/page-map'
import type { FC, ReactNode } from 'react'
import './globals.css'
import { ColibriProvider } from '@colibriwallet/sdk/react'
import Script from 'next/script'

const APP_NAME = 'Colibri'
const APP_URL = 'https://colibri.diy'
const APP_TITLE = 'Colibri – DIY crypto hardware wallet using ESP32'
const APP_DESCRIPTION =
  'DIY crypto hardware wallet, based on cheap ESP32 board - build yours easily with Arduino IDE, starting at $4 in parts'
const LINK_GITHUB = 'https://github.com/xtools-at/colibri'
const LINK_MATRIX = 'https://matrix.to/#/#colibriwallet:matrix.org'
const LINK_LEMMY = 'https://programming.dev/c/colibri'

export const metadata: Metadata = {
  metadataBase: new URL(APP_URL),
  title: {
    default: APP_TITLE,
    template: '%s | Colibri.diy',
  },
  description: APP_DESCRIPTION,
  applicationName: APP_NAME,
  generator: null,
  keywords: [
    'hardware wallet',
    'blockchain',
    'crypto',
    'esp32',
    'arduino',
    'wallet',
    'diy',
  ],
  creator: '@xtools-at',
  alternates: {
    // https://github.com/vercel/next.js/discussions/50189#discussioncomment-10826632
    canonical: './',
  },
  icons: {
    icon: '/icon.png',
    apple: '/apple-icon.png',
  },
  manifest: '/manifest.json',
  openGraph: {
    // https://github.com/vercel/next.js/discussions/50189#discussioncomment-10826632
    url: './',
    siteName: APP_TITLE,
    locale: 'en_US',
    type: 'website',
    images: [
      {
        url: '/opengraph-image.png',
      },
    ],
  },
  twitter: {
    card: 'summary_large_image',
    site: APP_URL,
    creator: '@xtools_',
    images: '/opengraph-image.png',
  },
  // verification: {},
  appleWebApp: {
    capable: true,
    title: APP_NAME,
    // statusBarStyle: 'black-translucent',
  },
  formatDetection: { telephone: false },
  // appLinks: {},
  other: {
    'msapplication-TileColor': '#009688',
    'msapplication-TileImage': `${APP_URL}/ms-icon.png`,
  },
}

const banner = undefined
/*
const banner = (
  <Banner dismissible={false}>
    🚧 Notification{' '}
    <Link href="https://github.com/xtools-at/colibri">
      link
    </Link>
    .
  </Banner>
)
*/

const navbar = (
  <Navbar
    logo={
      <Image
        src="/logo.png"
        alt=""
        className={cn(
          '[mask-image:linear-gradient(60deg,#000_25%,rgba(0,0,0,.2)_50%,#000_75%)] [mask-position:0] [mask-size:400%]',
          'hover:[mask-position:100%] hover:[transition:mask-position_1s_ease]',
          'w-auto'
        )}
        style={{ maxHeight: '66px' }}
      />
    }
    projectLink={LINK_GITHUB}
    projectIcon={<GithubLogo height="24" />}
    chatLink={LINK_MATRIX}
    chatIcon={<MatrixLogo height="24" />}
  />
)

const SocialIcon: FC<{
  children: ReactNode
  href: string
  title: string
  rel?: string
}> = ({ children, href, title, rel }) => (
  <Link
    href={href}
    title={title}
    className="mx-2"
    rel={rel || 'noreferrer noopener'}
  >
    {children}
  </Link>
)

const footer = (
  <Footer className="flex-col items-center md:items-start">
    <div className="flex flex-row">
      <SocialIcon
        title="Check out the code on Github"
        href={LINK_GITHUB}
        rel="noopener"
      >
        <GithubLogo height="24" />
      </SocialIcon>
      <SocialIcon
        title="Chat on Matrix: #colibriwallet:matrix.org"
        href={LINK_MATRIX}
      >
        <MatrixLogo height="24" />
      </SocialIcon>
      <SocialIcon title="Join the Lemmy community" href={LINK_LEMMY}>
        <LemmyLogo height="24" />
      </SocialIcon>
      <SocialIcon
        title="Chat on Telegram"
        href="https://t.me/+1vRfa1R5kUViYzM0"
      >
        <TelegramLogo height="24" />
      </SocialIcon>
      <SocialIcon
        title="Connect on Bluesky: @xtools.at"
        href="https://bsky.app/profile/xtools.at"
      >
        <BlueskyLogo height="24" />
      </SocialIcon>
      <SocialIcon
        title="Connect on Mastodon: @xtools"
        href="https://hachyderm.io/@xtools"
        rel="me noreferrer noopener"
      >
        <MastodonLogo height="24" />
      </SocialIcon>
    </div>
    <p className="mt-6 text-center text-xs md:text-left">
      © 2024-{new Date().getFullYear()}{' '}
      <Link href={LINK_GITHUB} rel="noopener">
        The Colibri Project
      </Link>
      . Created by{' '}
      <Link href="https://github.com/xtools-at" rel="noopener">
        xtools-at
      </Link>
      . Licensed under{' '}
      <Link href={`${LINK_GITHUB}/blob/main/LICENSE.md`} rel="noopener">
        AGPL-3.0
      </Link>
      .
    </p>
  </Footer>
)

const microdata = (
  <script type="application/ld+json">
    {JSON.stringify({
      '@context': 'http://schema.org/',
      '@type': 'Organization',
      name: 'The Colibri Project',
      logo: `${APP_URL}/icon.png`,
      url: APP_URL,
      sameAs: [LINK_GITHUB, LINK_LEMMY, LINK_MATRIX],
    })}
  </script>
)

const RootLayout: FC<{
  children: ReactNode
}> = async ({ children }) => {
  return (
    <ColibriProvider>
      <html lang="en" dir="ltr" suppressHydrationWarning>
        <Head
          color={{
            hue: { light: 174, dark: 174 },
            saturation: { light: 63, dark: 63 },
            lightness: { light: 40, dark: 40 },
          }}
        >
          {microdata}
        </Head>
        <body>
          <Layout
            banner={banner}
            navbar={navbar}
            pageMap={await getPageMap()}
            docsRepositoryBase="https://github.com/xtools-at/colibri/tree/main/apps/web"
            editLink="Improve this page on GitHub"
            sidebar={{ defaultMenuCollapseLevel: 1 }}
            footer={footer}
          >
            {children}
          </Layout>
          <Script src="https://scripts.simpleanalyticscdn.com/latest.js" />
        </body>
      </html>
    </ColibriProvider>
  )
}

export default RootLayout
