#���һ����Ŀ
{
	'targets': [
	{
		'target_name': 'libsanhero',	#��Ŀ����
		'type': 'static_library',		#static_library|shared_library|executable
		'sources': [
			'san_world.cpp',
			'san_world.h',
			'base_sprite.cpp',
			'base_sprite.h',
		],
		'dependencies': [	#����������gyp��Ŀ
		
		],
		'include_dirs': [
			#'$(MSBuildProgramFiles32)/Microsoft SDKs/Windows/v7.1A/include',
			'..',
			'../classes',
			'../cocos2d/cocos',
			'../cocos2d/cocos/audio/include',
			'../cocos2d/cocos/editor-support',
			'../cocos2d/external/glfw3/include/win32',
			'../cocos2d/external/win32-specific/gles/include/OGLES',
			'../cocos2d/external/freetype2/include/win32/freetype2',
			'../cocos2d/external/freetype2/include/win32/',
			'../cocos2d/external',
		],
		'defines': [	#Ԥ�����
			'WIN32',
			'_DEBUG',
			'_WINDOWS',
			'_USE_MATH_DEFINES',
			'GL_GLEXT_PROTOTYPES',
			'CC_ENABLE_CHIPMUNK_INTEGRATION=1',
			'_CRT_SECURE_NO_WARNINGS',
			'_SCL_SECURE_NO_WARNINGS',
		],
		# ָ�����λ��
		'configurations': {
			'DEBUG': {
				'msvs_settings': {
					'VCLibrarianTool': {
						'OutputFile': '../proj.win32/debug.win32/$(ProjectName).lib',
					},
				},
			},
			'RELEASE': {
				'msvs_settings': {
					'VCLibrarianTool': {
						'OutputFile': '../proj.win32/release.win32/$(ProjectName).lib',
					},
				},
			},
		},
		'msvs_settings': {	#��Ŀ¼������lib
			'VCLinkerTool': {
				'AdditionalLibraryDirectories': [
					'debug.win32',
					'../cocos2d/external/png/prebuilt/win32',
					'../cocos2d/external/jpeg/prebuilt/win32',
					'../cocos2d/external/tiff/prebuilt/win32',
					'../cocos2d/external/glfw3/prebuilt/win32',
					'../cocos2d/external/chipmunk/prebuilt/win32/release-lib',
					#'../cocos2d/external/curl/prebuilt/win32',
					#'../cocos2d/external/websockets/prebuilt/win32',
				],
				'AdditionalDependencies': [
					'libpng.lib',
					'libjpeg.lib',
					'libtiff.lib',
					'glfw3.lib',
					'libchipmunk.lib',
					'libcocos2d.lib',
					#'libcurl_imp.lib',
					#'websockets.lib',
				],

			},
		},
		'actions': [
			{
				'action_name': 'xcopy_resource',	#not list
				'inputs': [
				],
				'outputs': [
				],
				'action' : [
					'copy_resource.bat',
					'$(OutDir)',
				],
			},
			#{
			#	'source': [
			#		'copy_resource.bat',
			#	],
			#},
		],
	
	},
	],
}