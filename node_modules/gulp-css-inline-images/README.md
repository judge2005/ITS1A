gulp-css-inline-images
======================

Wrapper for using css-inline-images in [Gulp](http://gulpjs.com)

##Installation
Install like any other module.
```bash
npm install gulp-css-inline-images
```

##Usage
```javascript
var cssInlineImages = require('gulp-css-inline-images');

gulp.task('inline-images', function () {
    gulp.src('*.css')
        .pipe(cssInlineImages({
            webRoot: 'web',
            path: 'images'
        }))
        .pipe(gulp.dest('build'));
});
```

For detail usage off the css-inline-images module and its options [see the GitHub page](https://github.com/driebit/css-inline-images).
