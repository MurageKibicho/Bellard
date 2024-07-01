#include "mongoose.h"
#include "cJSON.h" 
#include "Tenant.h"
//clear && gcc Receipt.c mongoose.c cJSON.c -lm -o m.o && ./m.o


static void ServerHandler(struct mg_connection *connection, int event, void *eventData)
{
	if(event == MG_EV_HTTP_MSG)
	{
		struct mg_http_message *hm = (struct mg_http_message *) eventData;
		/*Enable Cors https://github.com/cesanta/mongoose/discussions/1860*/
		if(mg_vcmp(&hm->method, "OPTIONS") == 0)
		{
			printf("Preflight to enable cors\n");
			mg_http_reply(connection, 204, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\nAccess-Control-Allow-Headers: *\r\n\r\n","No Content");
                }
                
		if(mg_http_match_uri(hm, "/api/hi"))
		{
			mg_http_reply(connection, 200, "", "{%m:%m,%m:%m}\n", MG_ESC("uri"), mg_print_esc, hm->uri.len, hm->uri.ptr,MG_ESC("body"), mg_print_esc, hm->body.len, hm->body.ptr);
		}
		else if(mg_http_match_uri(hm, "/api/getAllLandlordDetails"))
		{
			/*create cJSON object*/
			cJSON *landlordDetailsJSON = cJSON_CreateObject(); 
			/*add strings to the cJSON object*/
			cJSON_AddStringToObject(landlordDetailsJSON, "landlordName", "EMMANUEL ALAIN MURAGE"); 
			cJSON_AddStringToObject(landlordDetailsJSON, "landlordPhoneNumber", "+254 757 012296"); 
			cJSON_AddStringToObject(landlordDetailsJSON, "landlordEmail", "murage.kibicho@karenhilltopgardens.com"); 
			cJSON_AddStringToObject(landlordDetailsJSON, "landlordCompany", "Karen Hilltop Gardens"); 
			cJSON_AddStringToObject(landlordDetailsJSON, "landlordBankName", "EQUITY BANK"); 
			cJSON_AddStringToObject(landlordDetailsJSON, "landlordBankLocation", "KILIMANI"); 
			cJSON_AddStringToObject(landlordDetailsJSON, "landlordBankAccountNumber", "97858488484"); 
			cJSON_AddStringToObject(landlordDetailsJSON, "companyLocation", "Dagoretti Road, Karen, Nairobi"); 
			
			/*convert cJSON object to string*/
			char *jsonResponse = cJSON_Print(landlordDetailsJSON);
			mg_printf(connection,
                	"HTTP/1.1 200 OK\r\n"
                	"Content-Type: application/json\r\n"
                	"Access-Control-Allow-Origin: *\r\n"
                	"Content-Length: %d\r\n\r\n"
                	"%s",
                	(int)strlen(jsonResponse), jsonResponse);
			/*Free memory*/
			cJSON_Delete(landlordDetailsJSON);
			free(jsonResponse);
		}
		else if(mg_http_match_uri(hm, "/api/getAllTenantDetails"))
		{
			int tenantIndex  = 10;

			/*create cJSON object*/
			cJSON *tenantDetailsJSON = cJSON_CreateObject(); 
			/*add strings to the cJSON object*/
			cJSON_AddStringToObject(tenantDetailsJSON, "tenantName", tenants[tenantIndex].tenantName);
			cJSON_AddStringToObject(tenantDetailsJSON, "tenantHouseNumber",tenants[tenantIndex].houseNumber);  
			cJSON_AddStringToObject(tenantDetailsJSON, "tenantPhoneNumber",tenants[tenantIndex].mobilePhoneNumber); 
			cJSON_AddStringToObject(tenantDetailsJSON, "tenantInvoiceDate",tenants[tenantIndex].invoiceDate); 
			cJSON_AddStringToObject(tenantDetailsJSON, "tenantDueDate",tenants[tenantIndex].dueDate); 
			cJSON_AddStringToObject(tenantDetailsJSON, "tenantRent",tenants[tenantIndex].rent); 
			cJSON_AddStringToObject(tenantDetailsJSON, "tenantServiceCharge", tenants[tenantIndex].serviceCharge); 
			cJSON_AddStringToObject(tenantDetailsJSON, "tenantWaterBill",tenants[tenantIndex].waterBill); 
			cJSON_AddStringToObject(tenantDetailsJSON, "tenantSubtotal",tenants[tenantIndex].subtotal); 
			cJSON_AddStringToObject(tenantDetailsJSON, "tenantSubtotalBeforeTax",tenants[tenantIndex].subtotalBeforeTax); 
			cJSON_AddStringToObject(tenantDetailsJSON, "tenantTaxRate",tenants[tenantIndex].taxRate); 
			cJSON_AddStringToObject(tenantDetailsJSON, "tenantTotalTax", tenants[tenantIndex].totalTax); 
			cJSON_AddStringToObject(tenantDetailsJSON, "tenantBalanceDue",tenants[tenantIndex].balanceDue); 
			
			/*convert cJSON object to string*/
			char *jsonResponse = cJSON_Print(tenantDetailsJSON);
			mg_printf(connection,
                	"HTTP/1.1 200 OK\r\n"
                	"Content-Type: application/json\r\n"
                	"Access-Control-Allow-Origin: *\r\n"
                	"Content-Length: %d\r\n\r\n"
                	"%s",
                	(int)strlen(jsonResponse), jsonResponse);
			/*Free memory*/
			cJSON_Delete(tenantDetailsJSON);
			free(jsonResponse);
		}
		else if(mg_http_match_uri(hm, "/api/logo"))
		{
			struct mg_http_serve_opts opts = 
			{
				.mime_types = "jpg=image/jpg",
				.extra_headers = "AA: bb\r\nCC: dd\r\n"
			};
      			mg_http_serve_file(connection, hm, "./Logo/logo.jpg", &opts);
		}
		else if(mg_http_match_uri(hm, "/api/companyName"))
		{
			const char *response = "Karen Hilltop Gardens";
			mg_printf(connection,
                	"HTTP/1.1 200 OK\r\n"
                	"Content-Type: text/plain\r\n"
                	"Access-Control-Allow-Origin: *\r\n"
                	"Content-Length: %d\r\n\r\n"
                	"%s",
                	(int)strlen(response), response);
		}
		else if(mg_http_match_uri(hm, "/api/location"))
		{
			const char *response = "Dagoretti Road, Karen, Nairobi";
			mg_printf(connection,
                	"HTTP/1.1 200 OK\r\n"
                	"Content-Type: text/plain\r\n"
                	"Access-Control-Allow-Origin: *\r\n"
                	"Content-Length: %d\r\n\r\n"
                	"%s",
                	(int)strlen(response), response);
		}
		else if(mg_http_match_uri(hm, "/api/email"))
		{
			const char *response = "murage.kibicho@karenhilltopgardens.com";
			mg_printf(connection,
                	"HTTP/1.1 200 OK\r\n"
                	"Content-Type: text/plain\r\n"
                	"Access-Control-Allow-Origin: *\r\n"
                	"Content-Length: %d\r\n\r\n"
                	"%s",
                	(int)strlen(response), response);
		}
		else if(mg_http_match_uri(hm, "/api/phoneNumber"))
		{
			const char *response = "+254 757012297";
			mg_printf(connection,
                	"HTTP/1.1 200 OK\r\n"
                	"Content-Type: text/plain\r\n"
                	"Access-Control-Allow-Origin: *\r\n"
                	"Content-Length: %d\r\n\r\n"
                	"%s",
                	(int)strlen(response), response);
		}
		else if(mg_http_match_uri(hm, "/api/bankBranch"))
		{
			const char *response = "KAREN";
			mg_printf(connection,
                	"HTTP/1.1 200 OK\r\n"
                	"Content-Type: text/plain\r\n"
                	"Access-Control-Allow-Origin: *\r\n"
                	"Content-Length: %d\r\n\r\n"
                	"%s",
                	(int)strlen(response), response);
		}
		else if(mg_http_match_uri(hm, "/api/bankAccountNumber"))
		{
			const char *response = "32456753452";
			mg_printf(connection,
                	"HTTP/1.1 200 OK\r\n"
                	"Content-Type: text/plain\r\n"
                	"Access-Control-Allow-Origin: *\r\n"
                	"Content-Length: %d\r\n\r\n"
                	"%s",
                	(int)strlen(response), response);
		}
		else if(mg_http_match_uri(hm, "/api/bankAccountName"))
		{
			const char *response = "EMMANUEL ALAIN MURAGE";
			mg_printf(connection,
                	"HTTP/1.1 200 OK\r\n"
                	"Content-Type: text/plain\r\n"
                	"Access-Control-Allow-Origin: *\r\n"
                	"Content-Length: %d\r\n\r\n"
                	"%s",
                	(int)strlen(response), response);
		}
		else
		{
			struct mg_http_serve_opts opts = {.root_dir = "dist"};
			mg_http_serve_dir(connection, hm, &opts);
		}
	}
	
}
int main()
{	
	numberOfTenants = sizeof(tenants) / sizeof(tenants[0]);
	struct mg_mgr eventManager;mg_mgr_init(&eventManager);
	//mg_log_set(MG_LL_DEBUG);
	printf("Landlord server running on port 8000\n");
	mg_http_listen(&eventManager, "http://0.0.0.0:8000", ServerHandler, NULL);
	for (;;) mg_mgr_poll(&eventManager, 1000); 
	
	mg_mgr_free(&eventManager);
  	return 0;
}
