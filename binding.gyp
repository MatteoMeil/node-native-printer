{
  "targets": [
    {
      "target_name": "unix_printer",
      "sources": ["./lib/unix/methods.cpp", "./lib/unix/API.cpp"],
      'cflags': ['-fexceptions'],
      'cflags_cc': ['-fexceptions'],
      "conditions": [
        ['OS=="mac"', {
          'xcode_settings': {
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'
          }
        }],
        ['OS!="win"', {
          'cflags':[
            '<!(cups-config --cflags)'
          ],
          'ldflags':[
            '<!(cups-config --libs)'
            #'-lcups -lgssapi_krb5 -lkrb5 -lk5crypto -lcom_err -lz -lpthread -lm -lcrypt -lz'
          ],
          'libraries':[
            '<!(cups-config --libs)'
            #'-lcups -lgssapi_krb5 -lkrb5 -lk5crypto -lcom_err -lz -lpthread -lm -lcrypt -lz'
          ],
          'link_settings': {
              'libraries': [
                  '<!(cups-config --libs)'
              ]
           }
        }]
      ]
    }
  ]
}
