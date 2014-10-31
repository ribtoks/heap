var contextMenu = require("sdk/context-menu");
var clipboard = require("sdk/clipboard");

var menuItem = contextMenu.Item({
    label: "Copy image title",
    context: contextMenu.SelectorContext("img"),
    contentScript: 'self.on("click", function (node) {' +
        '  var text = node.title;' +
        '  self.postMessage(text);' +
        '});',
    onMessage: function (imageTitle) {
        clipboard.set(imageTitle)
    }
});

