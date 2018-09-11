css-inline-images
=================

Simple node module for inlining images as base64 strings in CSS.

##Installation
Install with NPM like any other package
```bash
npm install css-inline-images
```

##Usage
Append `?embed` to the urls of the images that you want inlined

```css
.leave-me-alone {
    background-image: url(file.gif);
}

.embed-me {
    background-image: url(file.gif?embed)
}
```

The module will search for `url()` declarations in your CSS code so its use is not limited to background images.

##Options
* `webRoot`
  The web root of your web project. This property will be used as a start point for all file paths.
  This is enough for absolute urls (e.g.  `url(/images/file.gif?embed)`)

* `path`
  Used in addition to `webRoot` to determine the paths for relative urls (e.g. `url(images/file.gif?embed)`)

###Example
With these options the following paths will be used to locate the images in the filesystem
```javascript
{
    webRoot: 'web',
    path: 'otherImages'
}
```
* `url(/images/file.gif?embed)` => `web/images/file.gif`
* `url(file.gif?embed)` => `web/otherImages/file.gif`
