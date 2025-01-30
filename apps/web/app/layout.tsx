import { NextraLogo, VercelLogo } from '@components/icons'
import cn from 'clsx'
import type { Metadata } from 'next'
import { Footer, Layout, Link, Navbar } from 'nextra-theme-docs'
import { Banner, Head } from 'nextra/components'
import { getPageMap } from 'nextra/page-map'
import type { FC, ReactNode } from 'react'
import './globals.css'
import { BleProvider } from 'contexts/BleContext'

export const metadata: Metadata = {
  description: 'Make beautiful websites with Next.js & MDX.',
  metadataBase: new URL('https://nextra.site'),
  keywords: [
    'Nextra',
    'Next.js',
    'React',
    'JavaScript',
    'MDX',
    'Markdown',
    'Static Site Generator',
  ],
  generator: 'Next.js',
  applicationName: 'Nextra',
  appleWebApp: {
    title: 'Nextra',
  },
  title: {
    default: 'Nextra – Next.js Static Site Generator',
    template: '%s | Nextra',
  },
  openGraph: {
    // https://github.com/vercel/next.js/discussions/50189#discussioncomment-10826632
    url: './',
    siteName: 'Nextra',
    locale: 'en_US',
    type: 'website',
  },
  other: {
    'msapplication-TileColor': '#fff',
  },
  twitter: {
    site: 'https://nextra.site',
  },
  alternates: {
    // https://github.com/vercel/next.js/discussions/50189#discussioncomment-10826632
    canonical: './',
  },
}

const banner = (
  <Banner dismissible={false}>
    🚧 This is WIP documentation for Nextra 4.0. Dima Machina is looking{' '}
    <Link href="https://github.com/dimaMachina">
      for a new job or consulting
    </Link>
    .
  </Banner>
)
const navbar = (
  <Navbar
    logo={
      <NextraLogo
        height="20"
        className={cn(
          '[mask-image:linear-gradient(60deg,#000_25%,rgba(0,0,0,.2)_50%,#000_75%)] [mask-position:0] [mask-size:400%]',
          'hover:[mask-position:100%] hover:[transition:mask-position_1s_ease]'
        )}
      />
    }
    projectLink="https://github.com/shuding/nextra"
  />
)
const footer = (
  <Footer className="flex-col items-center md:items-start">
    <a
      className="x:focus-visible:nextra-focus flex items-center gap-1"
      target="_blank"
      rel="noreferrer"
      title="vercel.com homepage"
      href="https://vercel.com?utm_source=nextra.site"
    >
      Powered by
      <VercelLogo height="20" />
    </a>
    <p className="mt-6 text-xs">
      © {new Date().getFullYear()} The Colibri Project.
    </p>
  </Footer>
)

const RootLayout: FC<{
  children: ReactNode
}> = async ({ children }) => {
  return (
    <BleProvider>
      <html lang="en" dir="ltr" suppressHydrationWarning>
        <Head />
        <body>
          <Layout
            banner={banner}
            navbar={navbar}
            pageMap={await getPageMap()}
            docsRepositoryBase="https://github.com/shuding/nextra/tree/main/docs"
            editLink="Edit this page on GitHub"
            sidebar={{ defaultMenuCollapseLevel: 1 }}
            footer={footer}
          >
            {children}
          </Layout>
        </body>
      </html>
    </BleProvider>
  )
}

export default RootLayout
