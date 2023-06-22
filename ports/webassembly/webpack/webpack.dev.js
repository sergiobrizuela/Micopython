const { merge } = require('webpack-merge');
const common = require('./webpack.common.js');

module.exports = merge(common, {
    mode: 'development',
    output: {
        publicPath: '/'
    },
    devtool: 'inline-source-map',
    devServer: {
        compress: true,
        port: 8080,
        hot: false,
    }
});