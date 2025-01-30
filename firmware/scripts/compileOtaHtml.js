/* eslint-disable @typescript-eslint/no-require-imports */
const fs = require('fs')
const path = require('path')
const zlib = require('zlib')

const files = [
  {
    htmlIn: 'src/core/ota/ElegantOTA/otaHtml.html',
    htmlOut: 'data/index.html',
    cOut: 'src/core/ota/ElegantOTA/otaHtml.h',
    cVar: 'OTA_PAGE_HTML',
    cConfig: '../../../../config.h',
  },
  {
    htmlIn: 'src/core/ota/portalHtml.html',
    htmlOut: 'data/portal/index.html',
    cOut: 'src/core/ota/portalHtml.h',
    cVar: 'OTA_PORTAL_HTML',
    cConfig: '../../../config.h',
  },
]

function replaceTags(html, oldTag, newContent) {
  const tagRegex = new RegExp(
    `<${oldTag}([^>]*)>[\\s\\S]*?<\\/${oldTag}>`,
    'gi'
  )
  return html.replace(tagRegex, newContent)
}

function toCppByteArray(buffer) {
  const byteArray = Array.from(buffer).map(
    (byte) => `0x${byte.toString(16).padStart(2, '0')}`
  )
  return `{
${byteArray.join(', ')}
}`
}

function processFiles(baseDir) {
  files.forEach((file) => {
    const filePath = path.join(baseDir, file.htmlIn)
    const fileContent = fs.readFileSync(filePath, 'utf8')
    const gzippedContent = zlib.gzipSync(fileContent)
    const byteArray = toCppByteArray(gzippedContent)

    // write gzipped header files
    const headerContent = `#pragma once
// generated file using 'pnpm build:ota', do not edit directly

#include "${file.cConfig}"

#if defined(OTA_ENABLED) && !defined(OTA_USE_LITTLEFS)

const size_t ${file.cVar}_LEN = ${gzippedContent.length};
const uint8_t ${file.cVar}[] = ${byteArray};

#endif
`
    fs.writeFileSync(path.join(baseDir, file.cOut), headerContent)

    // write LittleFS html files
    let fsHtml =
      '<!-- generated file using `pnpm build:ota`, do not edit directly -->\n' +
      replaceTags(
        fileContent,
        'script',
        '<script src="/ota.js" type="module" crossorigin></script>'
      )
    fsHtml = replaceTags(
      fsHtml,
      'style',
      '<link rel="stylesheet" href="/styles.css" />'
    )

    fs.writeFileSync(path.join(baseDir, file.htmlOut), fsHtml)
  })
}

processFiles('./firmware/colibri')
