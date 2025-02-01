import type { Config } from 'tailwindcss'

export default {
  content: [
    './app/**/*.{js,jsx,ts,tsx,md,mdx}',
    './pages/**/*.{js,jsx,ts,tsx,md,mdx}',
    './components/**/*.{js,jsx,ts,tsx,md,mdx}',
    './content/**/*.{md,mdx}',
    './src/**/*.{js,jsx,ts,tsx,md,mdx}',
  ],
  theme: {
    extend: {},
  },
  plugins: [],
} satisfies Config
