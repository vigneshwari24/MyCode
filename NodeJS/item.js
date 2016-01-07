var method = item.prototype;

function item(id,name,qtyAvailable,price) {
this.id = id;
this.name = name;
this.qtyAvailable = qtyAvailable;
this.price = price;

}

module.exports = item;
