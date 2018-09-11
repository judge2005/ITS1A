'use strict';

var fs = require('fs'),
    mimeTypes = {
        '.png': 'image/png',
        '.gif': 'image/gif',
        '.jpg': 'image/jpg',
        '.webp': 'image/webp',
        '.svg': 'image/svg+xml'
    },
    options = {},
    matchRegExp = /url\s*\(\s*["']?([^\)"']+)["']?\s*\)/gi;

function getUrlsToInline (content) {
    var urlsToInline = [],
        urlMatch,
        cleanUrl,
        url;

    while ((urlMatch = matchRegExp.exec(content)) !== null) {
        url = urlMatch[1];

        if (url.substr(-6, 6) == '?embed') {
            urlsToInline.push(url);
        }
    }

    return urlsToInline;
}

function findFsPath (url) {
    var retVal;

    if (url[0] == '/') {
        retVal = options.webRoot + url;
    } else if (url.indexOf('://') > -1) {
        console.warn('Skipping external url "%s"', url);
        retVal = false;
    } else {
        retVal = options.webRoot + options.path + '/' + url;
    }

    if (retVal) {
        retVal = retVal.replace('?embed', '');
    }

    return retVal;
}

function replaceUrlsWithBase64Data (content, urls) {
    urls.forEach(function (url) {
        var filePath = findFsPath(url),
            base64Data;

        if (filePath) {
            base64Data = getBase64DataForImage(filePath);
            content = content.replace(url, base64Data);
        }
    });

    return content;
}

function getBase64DataForImage (filePath) {
    var contentsBuffer = fs.readFileSync(filePath),
        extension      = filePath.substr(-4, 4),
        encoding       = 'base64',
        mimeType,
        content;

    mimeType = mimeTypes[extension] || null;

    if (mimeType === null) {
        throw new Error('Unsupported file extension "' + extension + '"');
    }

    content = contentsBuffer.toString(encoding);

    return 'data:' + mimeType + ';' + encoding + ',' + content;
}

module.exports = function (css, opts) {
    var urls,
        newCss,
        output;

    options = opts || {};
    options.webRoot = options.webRoot || '';
    options.path = options.path || '';

    urls = getUrlsToInline(css);

    newCss = replaceUrlsWithBase64Data(css, urls);

    return newCss;
};
