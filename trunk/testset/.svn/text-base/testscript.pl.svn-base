#!/usr/bin/perl

#use module
use XML::Simple;
use Data::Dumper;


use Cwd;

$xml = new XML::Simple;

$data = $xml->XMLin("testconfig.xml");
print Dumper($data);
print $data->{luacpath};


#Prepare SKIP hash
%skiplist;

$temparraysize = @{$data->{skip}}; 
if(defined $data->{skip}){
    if($temparraysize>1){
	foreach $toskip(@{$data->{skip}}){
	    print($data->{skip});
	    if(!(defined $skiplist{$toskip})){
		$skiplist{$toskip}=1;  
	    }
	}
    }
    
    else{
	$skiplist{$data->{skip}}=1;
    }
}

$compareflag=0;
if($ARGV[0] eq "compare"){
    $compareflag=1;
}
$alternateflag=0;
if($ARGV[0] eq "alternate"){
    $alternateflag=1;
    $data->{luacpath} = $data->{luacpath_alternate}
}

$dir = cwd;

opendir(DIR, $dir);
@FILES=readdir(DIR);

@luacfail;
@llvm_asfail;
@compfail;
@passlist;

$totalnumitems=0;

foreach $file(@FILES){
    if($file=~m/\.lua$/){
	if(!(defined $skiplist{$file})){
	    $totalnumitems=$totalnumitems+1;
	    $original = $file;
	    
	    $luaoutput = $file;
	    $luaoutput =~ s/\.lua$/\.out/;
	    #print("\n Generating LUA Output for".$file."\n");
	    #print("lua $file > $luaoutput\n");
	    #$myreturn = `lua $file > $luaoutput`;
	    
	    
	    
	    print("\n\nExecuting file ".$file." with LUAC\n");
	    print("$data->{luacpath} $file\n");
	    $myreturn = `$data->{luacpath} $file 2>&1`;
	    if($myreturn ne "Lexing & Parsing.\nPerforming type inference.\nGenerating llvm IR.\nFinished compiling.\n"){
		push(@luacfail,$original);
		next;
	    }
	    print("$myreturn\n");
	    $file=~s/\.lua$/\.ll/;
	    print("Executing file ".$file." with llvm-as\n");
	    print("llvm-as $file\n");
	    $myreturn = `llvm-as $file 2>&1`;
	    if($myreturn ne ''){
		push(@llvm_asfail,$original);
		next;
	    }
	    
	    
	    $luacoutput = $file;
	    $luacoutput =~ s/\.ll$/\.temp/;
	    $file=~s/\.ll$/\.bc/;
	    print("Executing file ".$file." with lli\n");
	    print("lli $file > $luacoutput\n");
	    $myreturn = `lli $file > $luacoutput`;
	    
	    $myreturn = `diff -w $luaoutput $luacoutput`;
	    if($myreturn ne ''){
		push(@compfail, $original);
	    }
	    else{
		push(@passlist, $original);
	    }
	    
	}
	
	
    }
}
#$dummy = `rm *.temp`;
$dummy = `rm *.ll`;
$dummy = `rm *.bc`;






if(defined $data->{luacpath_alternate} && $compareflag==1){
$dir2 = cwd;

opendir(DIR2, $dir2);
@FILES=readdir(DIR2);
@luacfail2;
@llvm_asfail2;
@compfail2;
@passlist2;

$totalnumitems=0;

foreach $file(@FILES){
    if($file=~m/\.lua$/){
	if(!(defined $skiplist{$file})){
	    $totalnumitems=$totalnumitems+1;
	    $original = $file;
	    
	    $luaoutput = $file;
	    $luaoutput =~ s/\.lua$/\.out/;
	    #print("\n Generating LUA Output for".$file."\n");
	    #print("lua $file > $luaoutput\n");
	    #$myreturn = `lua $file > $luaoutput`;
	    
	    
	    
	    print("\n\nExecuting file ".$file." with LUAC\n");
	    print("$data->{luacpath_alternate} $file\n");
	    $myreturn = `$data->{luacpath_alternate} $file 2>&1`;
	    if($myreturn ne "Lexing & Parsing.\nPerforming type inference.\nGenerating llvm IR.\nFinished compiling.\n"){
		push(@luacfail2,$original);
		next;
	    }
	    print("$myreturn\n");
	    $file=~s/\.lua$/\.ll/;
	    print("Executing file ".$file." with llvm-as\n");
	    print("llvm-as $file\n");
	    $myreturn = `llvm-as $file 2>&1`;
	    if($myreturn ne ''){
		push(@llvm_asfail2,$original);
		next;
	    }
	    
	    
	    $luacoutput = $file;
	    $luacoutput =~ s/\.ll$/\.temp/;
	    $file=~s/\.ll$/\.bc/;
	    print("Executing file ".$file." with lli\n");
	    print("lli $file > $luacoutput\n");
	    $myreturn = `lli $file > $luacoutput`;
	    
	    $myreturn = `diff -w $luaoutput $luacoutput`;
	    if($myreturn ne ''){
		push(@compfail2, $original);
	    }
	    else{
		push(@passlist2, $original);
	    }
	    
	}
	
	
    }
}
#$dummy = `rm *.temp`;
$dummy = `rm *.ll`;
$dummy = `rm *.bc`;

}









print("\n\n\n\n----------------------------------------------\n\n\n\n");
print("\n BEGINNING REPORT:\n\n");
$totalnumpass = $#passlist+1;
$totalnumluacfail = $#luacfail+1;
$totalnumllvm_asfail = $#llvm_asfail+1;
$totalnumcompfail = $#compfail+1;
print("Total Number of Items: $totalnumitems");
print("\nFiles that passed all stages ($totalnumpass)\n");
foreach $pass(@passlist){
    print("$pass\n");
}
print("\nFiles that failed at LUAC stage ($totalnumluacfail)\n");
foreach $fail(@luacfail){
    print("$fail\n");
}

print("\nFiles that failed at LLVM-AS stage ($totalnumllvm_asfail)\n");
foreach $fail(@llvm_asfail){
    print("$fail\n");
}

print("\nFiles that failed at LLI or COMPARE stage ($totalnumcompfail)\n");
foreach $fail(@compfail){
    print("$fail\n");
}



if(defined $data->{luacpath_alternate} && $compareflag==1){
    %passlist2_hash;
    %luacfail2_hash;
    %llvm_asfail2_hash;
    %compfail2_hash;
    %passlist_hash;
    %luacfail_hash;
    %llvm_asfail_hash;
    %compfail_hash;

    foreach $pass(@passlist2){
	$passlist2_hash{$pass}=1;
    }
    foreach $fail(@luacfail2){
	$luacfail2_hash{$fail}=1;
    }
    foreach $fail(@llvm_asfail2){
	$llvm_asfail2_hash{$fail}=1;
    }
    foreach $fail(@compfail2){
	$compfail2_hash{$fail}=1;
    }
    foreach $pass(@passlist){
	$passlist_hash{$pass}=1;
    }
    foreach $fail(@luacfail){
	$luacfail_hash{$fail}=1;
    }
    foreach $fail(@llvm_asfail){
	$llvm_asfail_hash{$fail}=1;
    }
    foreach $fail(@compfail){
	$compfail_hash{$fail}=1;
    }



print("\n\n\n\n----------------------------------------------\n\n\n\n");
    print("\n\n\n\n----------------------------------------------\n\n\n\n");
    print("\n BEGINNING OMPARISON:\n\n");
    print("Files in CURRENT but not on ALTERNATE\n\n");
    print("In PASSED:\n");
    foreach $pass(@passlist){
	if(!(defined $passlist2_hash{$pass})){
	    print("$pass\n");
	}
    }
    print("\nFiles that failed at LUAC stage \n");
    foreach $fail(@luacfail){
	if(!(defined $luacfail2_hash{$fail})){
	    print("$fail\n");
	}
    }
    
    print("\nFiles that failed at LLVM-AS stage \n");
    foreach $fail(@llvm_asfail){
	if(!(defined $llvm_asfail2_hash{$fail})){
	    print("$fail\n");
	}
    }
    
    print("\nFiles that failed at LLI or COMPARE stage\n");
    foreach $fail(@compfail){
	if(!(defined $compfail2_hash{$fail})){
	    print("$fail\n");
	}
    }

print("\n\n\n\n----------------------------------------------\n\n\n\n");
    
 print("Files in ALTERNATE but not on CURRENT\n\n");
  print("In PASSED:\n");
    foreach $pass(@passlist2){
	if(!(defined $passlist_hash{$pass})){
	    print("$pass\n");
	}
    }
    print("\nFiles that failed at LUAC stage \n");
    foreach $fail(@luacfail2){
	if(!(defined $luacfail_hash{$fail})){
	    print("$fail\n");
	}
    }
    
    print("\nFiles that failed at LLVM-AS stage \n");
    foreach $fail(@llvm_asfail2){
	if(!(defined $llvm_asfail_hash{$fail})){
	    print("$fail\n");
	}
    }
    
    print("\nFiles that failed at LLI or COMPARE stage\n");
    foreach $fail(@compfail2){
	if(!(defined $compfail_hash{$fail})){
	    print("$fail\n");
	}
    }

    print("CURRENT: $data->{luacpath}  ALTERNATE: $data->{luacpath_alternate}\n\n");


}
