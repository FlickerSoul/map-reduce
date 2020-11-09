The two tests implemented are the `word_count` and the `grep`. You can build the two tests by calling 

```
make word_count
make grep
```

I don't have statistics for `word_count`. 

For the `grap` test, the first test should have reduce counter retuern `1` since there is only one `Dictum` exists in the paragrahs; the second test should return `0` since `2.in` contains no input lines; the third test should have the reduce counter return `13` because there are thirteen `dictum` exists in the paragraph; the fourth test should have reduce counter of `0` since there does not exists `abc` phrase in the `1.in` file. 
