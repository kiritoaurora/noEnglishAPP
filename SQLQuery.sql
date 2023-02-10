--为review中的reviewDate创建索引
create NONCLUSTERED INDEX reDate on review(reviewDate ASC)
--查看各表索引
exec sp_helpindex review
exec sp_helpindex users
exec sp_helpindex word

-------------------------------------------------------------------------------------------
--存储过程1，获取用户待学单词，以十个为一组
-------------------------------------------------------------------------------------------
if exists(select name from sysobjects where name = 'newWords' and type = 'p')
drop procedure newWords
go
create procedure newWords
 @userName varchar(11)
 as    
	exec('if exists(select * from sysobjects where name = ''oldWords'' and type = ''v'')
	     drop view oldWords ')
    exec('create view oldWords as select * from review where userName = '+@userName)
	--select * from oldWords
	select TOP 10 word.englishWord,pa,chineseWord,englishInstance1,chineseInstance1,pron
    from word left join oldWords on word.englishWord = oldWords.englishWord
    where userName is null	
go
--测试
exec newWords '17586448158'
exec newWords '18725087325'
------------------------------------------------------------------------------------------
select * from word

------------------------------------------------------------------------------------------
--存储过程2，获取用户待学单词数
------------------------------------------------------------------------------------------
if exists(select name from sysobjects where name = 'numOfNewWord' and type = 'p')
drop procedure numOfNewWord
go
create procedure numOfNewWord
 @userName varchar(11)
 as    
	exec('if exists(select * from sysobjects where name = ''oldWords'' and type = ''v'')
	     drop view oldWords ')
    exec('create view oldWords as select * from review where userName = '+@userName)
	--select * from oldWords
	select count(*)
    from word left join oldWords on word.englishWord = oldWords.englishWord
    where userName is null	
go
--测试
exec numOfNewWord '17586448158'
exec numOfNewWord '18725087325'
------------------------------------------------------------------------------------------


------------------------------------------------------------------------------------------
--存储过程3，在复习表中插入新背的单词
------------------------------------------------------------------------------------------
if exists(select name from sysobjects where name = 'insertNewReviewWord' and type = 'p')
drop procedure insertNewReviewWord
go
create procedure insertNewReviewWord
 @userName varchar(11),@englishWord varchar(512)
 as    
    declare @reviewDate date
	set @reviewDate = DATEADD(day,1,GETDATE())
	insert into review(username,englishWord,reviewDate)
	values(@userName,@englishWord,@reviewDate)
go
--测试
exec insertNewReviewWord '17586448158','abrupt'
------------------------------------------------------------------------------------------


------------------------------------------------------------------------------------------
--存储过程4，获取用户当日需复习的单词
------------------------------------------------------------------------------------------
if exists(select name from sysobjects where name = 'reviewWords' and type = 'p')
drop procedure reviewWords
go
create procedure reviewWords
 @userName varchar(11)--,@nowDate date
 as    
	declare @nowDate date
	set @nowDate = (select convert(varchar(10),getdate(),120))

	select TOP 10 word.englishWord,pa,chineseWord,englishInstance1,chineseInstance1,pron
	from review,word
	where userName = @userName and reviewDate = @nowDate and review.englishWord = word.englishWord
go
--测试
--exec reviewWords '17586448158','2021-10-18'
exec reviewWords '18725087325'
------------------------------------------------------------------------------------------


------------------------------------------------------------------------------------------
--存储过程5，获取用户当日需复习的单词个数
------------------------------------------------------------------------------------------
if exists(select name from sysobjects where name = 'amountOfReviewWords' and type = 'p')
drop procedure amountOfReviewWords
go
create procedure amountOfReviewWords
 @userName varchar(11)--,@nowDate date
 as    
	declare @nowDate date
	set @nowDate = (select convert(varchar(10),getdate(),120))
	select count(*) 
	from review
	where userName = @userName and reviewDate = @nowDate
go
--测试
--exec amountOfReviewWords '17586448158','2021-10-18'
exec amountOfReviewWords '17586448158'
------------------------------------------------------------------------------------------


------------------------------------------------------------------------------------------
--存储过程6，更新复习成功的单词的复习次数,复习成功次数加一，反之不加
------------------------------------------------------------------------------------------
if exists(select name from sysobjects where name = 'updateSuReWordInfo' and type = 'p')
drop procedure updateSuReWordInfo
go
create procedure updateSuReWordInfo
 @userName varchar(11),@englishWord varchar(512)
 as    
	update review
	set reviewNumOfTimes = reviewNumOfTimes + 1
	where userName = @userName and englishWord = @englishWord
go
--测试
exec updateSuReWordInfo '18725087325','abnormal'
------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------
--存储过程7，更新复习失败的单词的复习日期，复习失败的单词第二天仍需复习
------------------------------------------------------------------------------------------
if exists(select name from sysobjects where name = 'updateDeReWordInfo' and type = 'p')
drop procedure updateDeReWordInfo
go
create procedure updateDeReWordInfo
 @userName varchar(11),@englishWord varchar(512)
 as    
    declare @reviewDate date
	set @reviewDate = (select reviewDate from review where userName = @userName and englishWord = @englishWord)
	select @reviewDate = DATEADD(day,1,@reviewDate)

	update review
	set reviewDate = @reviewDate
	where userName = @userName and englishWord = @englishWord
go
--测试
exec updateDeReWordInfo '18725087325','abnormal'
------------------------------------------------------------------------------------------


------------------------------------------------------------------------------------------
--存储过程8，对于没复习的单词，即：复习日期小于当日日期，更新其复习日期为当日
------------------------------------------------------------------------------------------
if exists(select name from sysobjects where name = 'slackWords' and type = 'p')
drop procedure slackWords
go
create procedure slackWords
 @userName varchar(11)
 as    
	update review
	set reviewDate = GETDATE()
	where userName = @userName and englishWord in (select englishWord 
													from review 
													where userName = @userName and reviewDate < GETDATE())
go
--测试
exec slackWords '18725087325'

------------------------------------------------------------------------------------------


------------------------------------------------------------------------------------------
--存储过程9，禁用review上的触发器
------------------------------------------------------------------------------------------
if exists(select name from sysobjects where name = 'disableTrigger' and type = 'p')
drop procedure disableTrigger
go
create procedure disableTrigger
as
    alter table review disable trigger all;
go
--测试
exec disableTrigger 
------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------
--存储过程10，启用review上的触发器
------------------------------------------------------------------------------------------
if exists(select name from sysobjects where name = 'enableTrigger' and type = 'p')
drop procedure enableTrigger
go
create procedure enableTrigger
as
    alter table review enable trigger all;
go
--测试
exec enableTrigger 
------------------------------------------------------------------------------------------


------------------------------------------------------------------------------------------
--触发器1，当复习单词的复习次数改变时，更新其复习时间，使用dateadd函数
--当复习次数为8时，将复习时间设为NULL，意为已掌握该单词，之后无需复习
------------------------------------------------------------------------------------------
if exists(select * from sysobjects where name = 'newReviewDate' and type = 'tr')
drop trigger newReviewDate
go
create trigger newReviewDate
on review
after update
as
  declare @userName varchar(11),@englishWord varchar(512),@reviewNumOfTimes int,@reviewDate date
  set @reviewNumOfTimes = (select reviewNumOfTimes from inserted)
  set @reviewDate = (select reviewDate from inserted)
  set @userName = (select userName from inserted)
  set @englishWord = (select englishWord from inserted)

  update review
   set reviewDate = 
     case when @reviewNumOfTimes = 1 or @reviewNumOfTimes = 2 then DATEADD(day,3,@reviewDate)
	 when @reviewNumOfTimes = 3 then DATEADD(day,7,@reviewDate)
	 when @reviewNumOfTimes = 4 then DATEADD(day,14,@reviewDate)
	 when @reviewNumOfTimes = 5 then DATEADD(day,21,@reviewDate)
	 when @reviewNumOfTimes = 6 then DATEADD(day,30,@reviewDate)
	 when @reviewNumOfTimes = 7 then DATEADD(day,40,@reviewDate)
	 when @reviewNumOfTimes = 8 then NULL
	 end
  where userName = @userName and englishWord = @englishWord
go

------------------------------------------------------------------------------------------





------------------------------------------------------------------------------------------
--存储过程8，
------------------------------------------------------------------------------------------

--测试

------------------------------------------------------------------------------------------