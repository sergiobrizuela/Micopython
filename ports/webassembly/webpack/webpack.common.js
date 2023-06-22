const CopyPlugin = require("copy-webpack-plugin");
const HtmlWebpackPlugin = require('html-webpack-plugin');
const MonacoWebpackPlugin = require('monaco-editor-webpack-plugin');
const MiniCssExtractPlugin = require("mini-css-extract-plugin");
const webpack = require('webpack');
const path = require('path');


const portDirectory = path.resolve(__dirname, '..');

module.exports = {
	entry: './frontend_src/main.js',
	output: {
		path: path.resolve(portDirectory, 'bundle_out'),
		filename: 'app.[contenthash].js',
        clean: true,
	},
	module: {
		rules: [
			{
				test: /\.css$/,
				use: [MiniCssExtractPlugin.loader, 'css-loader']
			},
            {
				test: /\.less$/,
				use: [MiniCssExtractPlugin.loader, 'css-loader', 'less-loader']
			},
			{
				test: /\.ttf$/,
				type: 'asset/resource'
			}
		]
	},
	plugins: [
        new MonacoWebpackPlugin({
            languages: ['python']
        }),
        new HtmlWebpackPlugin({
            template: 'frontend.html',
            inject: 'body'
        }),
        new MiniCssExtractPlugin(),
        new CopyPlugin({
            patterns: [
                "lvgl.html",
                path.resolve(portDirectory, "build", "micropython.js"),
                path.resolve(portDirectory, "build", "firmware.wasm"),
                path.resolve(portDirectory, "docs", "wasm_file_api.js"),
            ]
        }),
        new webpack.DefinePlugin({
            'LV_BINDINGS_COMMIT_HASH': `"${process.env.LV_BINDINGS_COMMIT_HASH}"`
        })
    ]
};