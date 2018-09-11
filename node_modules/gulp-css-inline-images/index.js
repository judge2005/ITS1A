'use strict';

var cssInlineImages = require('css-inline-images'),
    PluginError     = require('gulp-util/lib/PluginError'),
    through         = require('through2'),

    pluginName      = 'gulp-css-inline-images';

module.exports = function (options) {
    return through.obj(function (file, encoding, done) {
        var oldCss, newCss;

        if (file.isNull()) {
			return done(null, file);
		}

        if (file.isStream()) {
            return done(new PluginError(pluginName, 'Streaming not supported', {
				fileName: file.path,
				showStack: false
			}));
        }

        oldCss = String(file.contents);
        newCss = cssInlineImages(oldCss, options);

        file.contents = new Buffer(newCss);

        this.push(file);
        done();
    });
};
