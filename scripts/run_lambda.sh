aws lambda invoke --function-name dns-validation --payload [ { "server": "8.8.8.8", "domain": "www.mikefilion.com" }, { "server": "1.1.1.1", "domain": "www.mikefilion.com" }, { "server": "8.8.8.8", "domain": "pegasus.mikefilion.com" } ] output.txt
