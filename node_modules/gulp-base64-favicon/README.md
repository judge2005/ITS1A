# [gulp][gulp]-base64-favicon [![npm version](https://badge.fury.io/js/gulp-base64-favicon.svg)][npm]

## Install

With [npm](https://npmjs.org/package/gulp-base64-favicon) do:

```
npm install gulp-base64-favicon --save-dev
```

## Example

```html
<head>
  <meta charset="utf-8">
  <!-- shortcut::favicon.png -->
  <link rel="stylesheet" href="style.css">
  ...
</head>  
```

```js
var gulp = require('gulp');
var favicon = require('gulp-base64-favicon');

gulp.task('default', function() {
    return gulp.src('./index.html') 
        .pipe(favicon())
        .pipe(gulp.dest('./out'));
});
```
In the output directory you will find your index.html with the following content:
```html
<head>
  <meta charset="utf-8">
  <link rel="shortcut icon" href="data:image/png;base64,iVBORw0KGgoAAAANSUh...sAAAAAElFTkSuQmCC"/>
  <link rel="stylesheet" href="style.css">
  ...
</head>  
```

## API

### favicon(src)

#### src
Type: `String`

Path to the source directory.

### favicon(obj)

#### obj
Type: `Object`

Container for data.

#### obj.src
Type: `String`

Path to the source directory.

## License

MIT © [Makarian Vladyslav](https://github.com/vldmkr)

[gulp]:    https://github.com/gulpjs/gulp
[npm]:     https://badge.fury.io/js/gulp-base64-favicon
