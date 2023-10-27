//Define which Sheet to enter data into 

var ss = SpreadsheetApp.openById('18BK3Ea-2ETcTq_aI6HfaKpuFkOKYSeFYYbjFpwpBLY'); 
var sheet = ss.getSheetByName('Temperature'); 
 
//Function to extract and enter data 
function doGet(e){ 
 
    //If data received is undefined send response back to ESP32   
    if (e.parameter == 'undefined') { 
    return ContentService.createTextOutput("Received data is undefined"); 
  } 
    //Create new DateTime variable 
    var dateTime= new Date();
    
    //Extract Data from JSON Object   
    temperature = e.parameters.temperature;   
    humidity   = e.parameters.humidity; 
    heatindex  = e.parameters.heatindex; 
 
    //Enter Data into Spreadsheet 
    //Get last updated row and enter data into next one   
    var nextRow = sheet.getLastRow() + 1; 
 
    //Enter Date and time into first two columns   
    sheet.getRange("A" + nextRow).setValue(dateTime);   
    sheet.getRange("B" + nextRow).setValue(dateTime); 
 
    //Enter weather data into other three columns   
    sheet.getRange("C" + nextRow).setValue(temperature);   
    sheet.getRange("D" + nextRow).setValue(humidity);   
    sheet.getRange("E" + nextRow).setValue(heatindex); 
 
  //returns response back to ESP32 
  return ContentService.createTextOutput("Data entered in sheet."); }
