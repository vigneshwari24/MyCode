require("console.table")
var jsonfile = require('jsonfile');
var item = require("./item.js");
var itemdisplay = require("./itemdisplay.js");
var fs = require("fs");
var files = fs.readdirSync(".");
var jsonfiles = new Array();
var cart = new Array();
var totalprice = 0;
var j = 0;
var itemname;
var qty = 0;
var price = 0;
var fichoice;
var calculatedflag = false;
var calculateflag = false;
var totaltopay = 0;
var itemlist = new Array();
var qtylist = new Array();
for(var i = 0; i < files.length; i++) {
	if(files[i].endsWith(".json")) {
		jsonfiles[j++] = files[i];
	}
}
console.log(jsonfiles);
console.table(jsonfiles);
var stdin = process.openStdin();
console.log("Please enter the file name to view the data:");
var d;
var i = 0;
var choiceValidation = 0;
var tabledisplay = true;
var keys = new Array();
var properchoice = false;
var qtyflag = false;

stdin.addListener("data", function (d) {
	if(d.toString().trim().toUpperCase() ==  'QUIT') d = "done";
					
	if(tabledisplay == true) {
		var file = d.toString().trim();
	
		obj = jsonfile.readFileSync(file,new item());
		var displayObject = new Array();
		for(var c = 0; c < obj.length; c++) {
			keys[c] = parseFloat(obj[c]['id'].toString());
			itemlist[c] = obj[c]['name'].toString();
			qtylist[c] = parseFloat("0");
		}
		//console.log(keys);
		console.table(obj);
		tabledisplay = false;
		console.log("please enter the id of your choice and quantity separated by space:");
		//d = "";
	} else {
		var arr = d.toString().trim().split(/[\s,]+/);
		if(arr.length != 2) {
			console.log("Please enter the input properly..");
		} else {
			var itemid = arr[0].trim();
			var dQ = arr[1].trim();
			var index = keys.indexOf(parseFloat(itemid));
			if(isNaN(index) || (index == -1)) {
			console.log("Please enter proper choice from the above list.. ");
			//console.log("Do you want the list to be displayed again?");
			} //console.log("Exiting fro else"); d = "done";
			else {
				//console.log("Proper id..");
				//console.log(obj[index]['qtyAvailable']);
				var aQ = parseFloat(obj[index]['qtyAvailable']);
				if(isNaN(dQ) || (parseFloat(dQ) <= 0)) {
					console.log("Please enter non zero positive numeric value for quantity ");
				} else {
					//numeric quantity
					if(parseFloat(dQ) > aQ) {
						console.log("The entered quantity is greater than the available qty");
					} else {
						var priceperitem = parseFloat(obj[index]['price']);
						//console.log(priceperitem + ' priceperitem');
						totaltopay = parseFloat(totaltopay) + parseFloat((dQ * priceperitem));
						var newqty = aQ - dQ;
						obj[index]['qtyAvailable'] = newqty;
						qtylist[index] = parseFloat(qtylist[index]) + parseFloat(dQ);
						console.log("Updated list");
						console.table(obj);
						//console.log(totaltopay);
						console.log("Do you want to continue shopping? " );
						console.log("If yes, please enter the desired id  and quantity (separated by space) to continue");
						console.log("Else type 'quit' to exit the application");
						}
				}
				
			}
		} 
		
		}
		
		
	if(d.toString().trim() == "done" ) {//|| i == 0) 
	console.log("Products bought");
	for(var counter = 0; counter < qtylist.length; counter++) {
		if(qtylist[counter] != 0) {
			console.log(itemlist[counter] + " : " + qtylist[counter]);
		}
	}
		console.log("The net total to pay.. " + totaltopay);
		console.log("Thank you for shopping with us!!..");
		process.exit();
	}
	//console.log("printing i" + i);
	i++;
	d = " ";
	
});
//console.log(d);
