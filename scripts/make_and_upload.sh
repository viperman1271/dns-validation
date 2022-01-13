make dns-validation-aws
make aws-lambda-package-dns-validation-aws
aws lambda update-function-code --function-name dns-validation --zip-file fileb://dns-validation-aws.zip
