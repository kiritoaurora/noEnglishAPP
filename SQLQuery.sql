--Ϊreview�е�reviewDate��������
create NONCLUSTERED INDEX reDate on review(reviewDate ASC)
--�鿴��������
exec sp_helpindex review
exec sp_helpindex users
exec sp_helpindex word

-------------------------------------------------------------------------------------------
--�洢����1����ȡ�û���ѧ���ʣ���ʮ��Ϊһ��
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
--����
exec newWords '17586448158'
exec newWords '18725087325'
------------------------------------------------------------------------------------------
select * from word

------------------------------------------------------------------------------------------
--�洢����2����ȡ�û���ѧ������
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
--����
exec numOfNewWord '17586448158'
exec numOfNewWord '18725087325'
------------------------------------------------------------------------------------------


------------------------------------------------------------------------------------------
--�洢����3���ڸ�ϰ���в����±��ĵ���
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
--����
exec insertNewReviewWord '17586448158','abrupt'
------------------------------------------------------------------------------------------


------------------------------------------------------------------------------------------
--�洢����4����ȡ�û������踴ϰ�ĵ���
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
--����
--exec reviewWords '17586448158','2021-10-18'
exec reviewWords '18725087325'
------------------------------------------------------------------------------------------


------------------------------------------------------------------------------------------
--�洢����5����ȡ�û������踴ϰ�ĵ��ʸ���
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
--����
--exec amountOfReviewWords '17586448158','2021-10-18'
exec amountOfReviewWords '17586448158'
------------------------------------------------------------------------------------------


------------------------------------------------------------------------------------------
--�洢����6�����¸�ϰ�ɹ��ĵ��ʵĸ�ϰ����,��ϰ�ɹ�������һ����֮����
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
--����
exec updateSuReWordInfo '18725087325','abnormal'
------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------
--�洢����7�����¸�ϰʧ�ܵĵ��ʵĸ�ϰ���ڣ���ϰʧ�ܵĵ��ʵڶ������踴ϰ
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
--����
exec updateDeReWordInfo '18725087325','abnormal'
------------------------------------------------------------------------------------------


------------------------------------------------------------------------------------------
--�洢����8������û��ϰ�ĵ��ʣ�������ϰ����С�ڵ������ڣ������临ϰ����Ϊ����
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
--����
exec slackWords '18725087325'

------------------------------------------------------------------------------------------


------------------------------------------------------------------------------------------
--�洢����9������review�ϵĴ�����
------------------------------------------------------------------------------------------
if exists(select name from sysobjects where name = 'disableTrigger' and type = 'p')
drop procedure disableTrigger
go
create procedure disableTrigger
as
    alter table review disable trigger all;
go
--����
exec disableTrigger 
------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------
--�洢����10������review�ϵĴ�����
------------------------------------------------------------------------------------------
if exists(select name from sysobjects where name = 'enableTrigger' and type = 'p')
drop procedure enableTrigger
go
create procedure enableTrigger
as
    alter table review enable trigger all;
go
--����
exec enableTrigger 
------------------------------------------------------------------------------------------


------------------------------------------------------------------------------------------
--������1������ϰ���ʵĸ�ϰ�����ı�ʱ�������临ϰʱ�䣬ʹ��dateadd����
--����ϰ����Ϊ8ʱ������ϰʱ����ΪNULL����Ϊ�����ոõ��ʣ�֮�����踴ϰ
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
--�洢����8��
------------------------------------------------------------------------------------------

--����

------------------------------------------------------------------------------------------