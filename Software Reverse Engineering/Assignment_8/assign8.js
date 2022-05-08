Interceptor.attach(Module.getExportByName(null, 'MessageBoxW'), {
	onEnter(args) {
    	args[2].writeUtf16String('Notepad');
    	console.log('MessageBoxW(): \n\tOriginal Title = ' + args[2].readUtf16String());
    	console.log('\tOriginal Caption = ' + args[1].readUtf16String());
    	args[1].writeUtf16String('Brought to you by Joseph');
    	args[2].writeUtf16String('Gotcha!');
	}
});
